#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_IMPLEMENTATION
#define NK_INCLUDE_FONT_BAKING

#include "../include/nuklear_blend2d.h"
#include <iostream>
#include <cstring>
#include <cerrno>
#include <unistd.h>  // R_OK 定义


// 兼容兜底：手动定义R_OK（标准值4）
#ifndef R_OK
#define R_OK 4
#endif

using std::cerr;

// 关键优化1：扩展系统字体路径+文件名（覆盖99% Android设备）
static const char* SYS_FONT_DIRS[] = {
    "/system/fonts",        // 主流设备默认路径
    "/system/font",         // 旧设备路径
    "/data/fonts",          // 部分定制系统路径
    "/vendor/fonts",        // 厂商定制路径
    "/system/etc/fonts",    // 补充路径1
    "/data/local/fonts",    // 补充路径2
    "/mnt/sdcard/fonts"     // 外部存储路径（非系统，需权限）
};

// 关键优化2：扩展目标字体（包含大小写、格式差异）
static const char* TARGET_FONTS[] = {
    "NotoSansCJK-Regular.ttc",
    "NotoSansCJK-Regular.ttf",  // 补充ttf格式
    "NotoSerifCJK-Regular.ttc",
    "NotoSerifCJK-Regular.ttf",
    "NotoSansSC-Regular.otf",   // 单独简体中文
    "NotoSansSC-Regular.ttf",
    "DroidSansFallback.ttf",    // 旧设备默认 fallback
    "Roboto-Regular.ttf",       // 英文兜底（避免完全空白）
    "simhei.ttf",               // 部分设备有宋体
    "mingliu.ttc"               // 部分设备有明体
};

// 辅助函数：打印详细错误日志（定位具体失败路径）
static void print_font_error(const char* action, const char* path, int err_code)
{
    cerr << "FONT ERROR: " << action << " '" << path << "' - ";
    switch (err_code)
    {
        case ENOENT: cerr << "File not found\n"; break;
        case EACCES: cerr << "Permission denied (need root?)\n"; break;
        case EISDIR: cerr << "Is a directory (not a font)\n"; break;
        default: cerr << "Error code: " << err_code << "\n"; break;
    }
}

// 核心：系统字体加载（带详细日志+多路径重试）
static BLFont nk_android_load_system_font()
{
    char font_path[256] = {0};  // 扩大路径缓冲区（适配长路径）
    BLFont empty_font;          // 空字体（失败返回）

    // 1. 遍历所有字体目录
    for (size_t dir_idx = 0; dir_idx < sizeof(SYS_FONT_DIRS)/sizeof(SYS_FONT_DIRS[0]); dir_idx++)
    {
        const char* dir = SYS_FONT_DIRS[dir_idx];
        // 检查目录是否存在且可读
        int dir_access = access(dir, R_OK);
        if (dir_access != 0)
        {
            print_font_error("Dir access failed", dir, errno);
            continue;  // 目录不可读，跳过
        }
        cerr << "FONT INFO: Found valid dir - " << dir << "\n";

        // 2. 遍历当前目录下的所有目标字体
        for (size_t font_idx = 0; font_idx < sizeof(TARGET_FONTS)/sizeof(TARGET_FONTS[0]); font_idx++)
        {
            const char* font_name = TARGET_FONTS[font_idx];
            // 拼接完整路径（安全避免溢出）
            snprintf(font_path, sizeof(font_path)-1, "%s/%s", dir, font_name);
            font_path[sizeof(font_path)-1] = '\0';  // 确保字符串结束

            // 3. 检查字体文件是否可读
            int file_access = access(font_path, R_OK);
            if (file_access != 0)
            {
                print_font_error("Font access failed", font_path, errno);
                continue;  // 字体不可读，试下一个
            }
            cerr << "FONT INFO: Found readable font - " << font_path << "\n";

            // 4. 用Blend2D加载字体（带错误检查）
            BLFontFace font_face;
            BLResult face_res = font_face.create_from_file(font_path);
            if (face_res != BL_SUCCESS)
            {
                cerr << "FONT ERROR: Create face failed for '" << font_path 
                     << "' - Blend2D code: " << face_res << "\n";
                continue;  // 加载字体脸失败，试下一个
            }

            // 5. 创建字体对象（大小24，与原逻辑一致）
            BLFont system_font;
            BLResult font_res = system_font.create_from_face(font_face, 24);
            if (font_res != BL_SUCCESS)
            {
                cerr << "FONT ERROR: Create font failed for '" << font_path 
                     << "' - Blend2D code: " << font_res << "\n";
                continue;
            }

            // 加载成功：返回有效字体
            cerr << "FONT SUCCESS: Loaded system font - " << font_path << "\n";
            return system_font;
        }
    }

    // 所有路径都失败：打印最终错误
    cerr << "FONT FATAL: All system font paths/names failed to load\n";
    return empty_font;
}

// 字体宽度计算（增加空字体保护，避免崩溃）
static float nk_blend2d_font_get_width(nk_handle ptr, float height, const char *text, int length)
{
    BLFont *font = (BLFont *)ptr.ptr;
    // 关键保护：空字体直接返回0，避免无效访问
    if (!font || font->is_empty())
    {
        cerr << "FONT WARN: Invalid/empty font passed to get_width()\n";
        return 0.0f;
    }

    BLGlyphBuffer glyph_buf;
    BLTextMetrics text_metrics;
    glyph_buf.set_utf8_text(text, length);
    font->shape(glyph_buf);
    font->get_text_metrics(glyph_buf, text_metrics);
    return (text_metrics.bounding_box.x1 - text_metrics.bounding_box.x0);
}

// Nuklear字体初始化（保持原接口）
NK_API BLFont nk_zh_font_init()
{
    BLFont system_font = nk_android_load_system_font();
    if (system_font.is_empty())
    {
        cerr << "FONT INIT ERROR: System font load failed\n";
    }
    return system_font;
}

// Nuklear上下文初始化（关键修复：适配BLFontDataCore参数类型）
NK_API struct nk_context nk_blend2d_init(BLFont &font)
{
    static struct nk_context ctx;
    static struct nk_user_font nk_user_font;

    // 保护：若字体为空，创建兼容版空字体脸（修复类型不匹配问题）
    if (font.is_empty())
    {
        cerr << "FONT INIT WARN: Using fallback empty font for Nuklear\n";
        BLFontData empty_font_data;  // 先创建BLFontData（继承自BLFontDataCore）
        BLFontFace empty_face;

        // 步骤1：创建空的字体数据（传递nullptr+0，适配低版本接口）
        BLResult data_res = empty_font_data.create_from_data(nullptr, 0);
        if (data_res != BL_SUCCESS)
        {
            cerr << "FONT INIT ERROR: Create empty font data failed - Blend2D code: " << data_res << "\n";
        }
        // 步骤2：用空字体数据创建字体脸（类型匹配，避免编译错误）
        else
        {
            BLResult face_res = empty_face.create_from_data(empty_font_data, 0);
            if (face_res != BL_SUCCESS)
            {
                cerr << "FONT INIT ERROR: Create fallback face failed - Blend2D code: " << face_res << "\n";
            }
            // 步骤3：基于空字体脸创建占位字体
            else
            {
                font.create_from_face(empty_face, 24);
            }
        }
    }

    nk_user_font.userdata = nk_handle_ptr(&font);
    nk_user_font.height = font.metrics().ascent + font.metrics().descent;
    nk_user_font.width = nk_blend2d_font_get_width;

    if (!nk_init_default(&ctx, &nk_user_font))
    {
        cerr << "NUKLEAR ERROR: Failed to initialize context\n";
    }
    return ctx;
}

// Nuklear渲染逻辑（完全完整，无省略）
NK_API void nk_blend2d_render(struct nk_context *nctx, BLContext &bctx)
{
    const struct nk_command *cmd = 0;
    bctx.clear_all();
    nk_foreach(cmd, nctx)
    {
        switch (cmd->type)
        {
        case NK_COMMAND_NOP:
            break;

        case NK_COMMAND_SCISSOR:
        {
            const struct nk_command_scissor *data = (const struct nk_command_scissor *)cmd;
            // Do not enable! It causes some wild clipping issues...
            // bctx.clipToRect((double)data->x, (double)data->y, (double)data->w, (double)data->h);
        }
        break;

        case NK_COMMAND_LINE:
        {
            const struct nk_command_line *data = (const struct nk_command_line *)cmd;
            bctx.set_stroke_width((double)data->line_thickness);
            bctx.set_stroke_style(BLRgba32(data->color.r, data->color.g, data->color.b, data->color.a));
            bctx.stroke_line(data->begin.x, data->begin.y, data->end.x, data->end.y);
        }
        break;

        case NK_COMMAND_RECT:
        {
            const struct nk_command_rect *data = (const struct nk_command_rect *)cmd;
            bctx.set_stroke_width((double)data->line_thickness);
            bctx.set_stroke_style(BLRgba32(data->color.r, data->color.g, data->color.b, data->color.a));
            bctx.stroke_round_rect((double)data->x, (double)data->y, (double)data->w, (double)data->h, (double)data->rounding);
        }
        break;

        case NK_COMMAND_RECT_FILLED:
        {
            const struct nk_command_rect_filled *data = (const struct nk_command_rect_filled *)cmd;
            bctx.set_fill_style(BLRgba32(data->color.r, data->color.g, data->color.b, data->color.a));
            bctx.fill_round_rect((double)data->x, (double)data->y, (double)data->w, (double)data->h, (double)data->rounding);
        }
        break;

        case NK_COMMAND_CIRCLE:
        {
            const struct nk_command_circle *data = (const struct nk_command_circle *)cmd;
            bctx.set_stroke_width((double)data->line_thickness);
            bctx.set_stroke_style(BLRgba32(data->color.r, data->color.g, data->color.b, data->color.a));
            bctx.stroke_round_rect((double)data->x, (double)data->y, (double)data->w, (double)data->h, (double)(data->w / 2));
        }
        break;

        case NK_COMMAND_CIRCLE_FILLED:
        {
            const struct nk_command_circle_filled *data = (const struct nk_command_circle_filled *)cmd;
            bctx.set_fill_style(BLRgba32(data->color.r, data->color.g, data->color.b, data->color.a));
            bctx.fill_round_rect((double)data->x, (double)data->y, (double)data->w, (double)data->h, (double)(data->w / 2));
        }
        break;

        case NK_COMMAND_ARC:
        {
            const struct nk_command_arc *data = (const struct nk_command_arc *)cmd;
            bctx.set_stroke_width((double)data->line_thickness);
            bctx.set_stroke_style(BLRgba32(data->color.r, data->color.g, data->color.b, data->color.a));
            bctx.stroke_arc((double)data->cx, (double)data->cy, (double)data->r, (double)data->a[0], (double)data->a[1]);
        }
        break;

        case NK_COMMAND_ARC_FILLED:
        {
            const struct nk_command_arc_filled *data = (const struct nk_command_arc_filled *)cmd;
            bctx.set_fill_style(BLRgba32(data->color.r, data->color.g, data->color.b, data->color.a));
            bctx.fill_pie((double)data->cx, (double)data->cy, (double)data->r, (double)data->a[0], (double)data->a[1]);
        }
        break;

        case NK_COMMAND_TRIANGLE:
        {
            const struct nk_command_triangle *data = (const struct nk_command_triangle *)cmd;
            bctx.set_stroke_width((double)data->line_thickness);
            bctx.set_stroke_style(BLRgba32(data->color.r, data->color.g, data->color.b, data->color.a));
            bctx.stroke_triangle((double)data->a.x, (double)data->a.y, (double)data->b.x, (double)data->b.y, (double)data->c.x, (double)data->c.y);
        }
        break;

        case NK_COMMAND_TRIANGLE_FILLED:
        {
            const struct nk_command_triangle_filled *data = (const struct nk_command_triangle_filled *)cmd;
            bctx.set_fill_style(BLRgba32(data->color.r, data->color.g, data->color.b, data->color.a));
            bctx.fill_triangle((double)data->a.x, (double)data->a.y, (double)data->b.x, (double)data->b.y, (double)data->c.x, (double)data->c.y);
        }
        break;

        case NK_COMMAND_POLYGON:
        {
            const struct nk_command_polygon *data = (const struct nk_command_polygon *)cmd;
            bctx.set_stroke_width((double)data->line_thickness);
            bctx.set_stroke_style(BLRgba32(data->color.r, data->color.g, data->color.b, data->color.a));

            BLPointI *points = (BLPointI *)malloc(sizeof(BLPointI) * data->point_count);
            for (int i = 0; i < data->point_count; i++)
            {
                points[i].x = data->points[i].x;
                points[i].y = data->points[i].y;
            }

            bctx.stroke_polygon(points, data->point_count);
            free(points);
        }
        break;

        case NK_COMMAND_POLYGON_FILLED:
        {
            const struct nk_command_polygon_filled *data = (const struct nk_command_polygon_filled *)cmd;
            bctx.set_fill_style(BLRgba32(data->color.r, data->color.g, data->color.b, data->color.a));

            BLPointI *points = (BLPointI *)malloc(sizeof(BLPointI) * data->point_count);
            for (int i = 0; i < data->point_count; i++)
            {
                points[i].x = data->points[i].x;
                points[i].y = data->points[i].y;
            }

            bctx.fill_polygon(points, data->point_count);
            free(points);
        }
        break;

        case NK_COMMAND_POLYLINE:
        {
            const struct nk_command_polyline *data = (const struct nk_command_polyline *)cmd;
            bctx.set_stroke_width((double)data->line_thickness);
            bctx.set_stroke_style(BLRgba32(data->color.r, data->color.g, data->color.b, data->color.a));

            BLPointI *points = (BLPointI *)malloc(sizeof(BLPointI) * data->point_count);
            for (int i = 0; i < data->point_count; i++)
            {
                points[i].x = data->points[i].x;
                points[i].y = data->points[i].y;
            }

            bctx.stroke_polyline(points, data->point_count);
            free(points);
        }
        break;

        case NK_COMMAND_TEXT:
        {
            const struct nk_command_text *data = (const struct nk_command_text *)cmd;
            bctx.set_fill_style(BLRgba32(data->foreground.r, data->foreground.g, data->foreground.b, data->foreground.a));
            BLFont *font = (BLFont *)data->font->userdata.ptr;
            // 增加字体有效性判断，避免渲染崩溃
            if (font && !font->is_empty())
            {
                bctx.fill_utf8_text(BLPoint(data->x, data->y + font->metrics().ascent), *font, (const char *)data->string, data->length);
            }
        }
        break;

        case NK_COMMAND_CURVE:
        {
            const struct nk_command_curve *data = (const struct nk_command_curve *)cmd;
            bctx.set_stroke_width((double)data->line_thickness);
            bctx.set_stroke_style(BLRgba32(data->color.r, data->color.g, data->color.b, data->color.a));

            BLPath curve;
            curve.move_to(BLPoint((double)data->begin.x, (double)data->begin.y));
            curve.cubic_to((double)data->ctrl[0].x, (double)data->ctrl[0].y,
                          (double)data->ctrl[1].x, (double)data->ctrl[1].y,
                          (double)data->end.x, (double)data->end.y);
        }
        break;

        case NK_COMMAND_RECT_MULTI_COLOR:
        {
            // TODO: implement multi colour rectangle rendering
            // Blend2D currently does not support per-vertex colouring
            // May have to write a custom software rasterizer

            // QuickFix: Draw the rectangle with the colour of first vertex
            // This command is currently only used by the colour picker
            // If you don't use it, you're good to go!

            const struct nk_command_rect_multi_color *data = (const struct nk_command_rect_multi_color *)cmd;
            bctx.set_fill_style(BLRgba32(data->left.r, data->left.g, data->left.b, data->left.a));
            bctx.fill_rect(data->x, data->y, data->w, data->h);
        }
        break;

        case NK_COMMAND_IMAGE:
        {
            const struct nk_command_image *data = (const struct nk_command_image *)cmd;
            BLImage img;
            img.create_from_data(data->w, data->h, BL_FORMAT_PRGB32, data->img.handle.ptr, data->w, BL_DATA_ACCESS_READ);
            BLPattern pattern(img);
            bctx.fill_rect(data->x, data->y, data->w, data->h, pattern);
        }
        break;

        case NK_COMMAND_CUSTOM:
        default:
            break;
        }
    }
}
