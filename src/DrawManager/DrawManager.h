//
// Created by Administrator on 2026/2/21.
//

#ifndef CPURENDERTEMPLATE_DRAWMANAGER_H
#define CPURENDERTEMPLATE_DRAWMANAGER_H

#include <blend2d.h>
#include <string>

class DrawManager {
public:
    /**
     * @brief 绘制带描边的水平居中文本 (高性能字形缓存版)
     * * @param ctx          Blend2D 绘制上下文
     * @param font         已初始化的字体对象 (建议预先 Load)
     * @param text         UTF-8 编码的文本字符串
     * @param centerX      水平居中的参考中心 X 坐标
     * @param y            文本绘制的垂直基准 Y 坐标
     * @param fillColor    文本主体的填充颜色
     * @param outlineColor 描边颜色 (建议带透明度以获得平滑效果)
     * @param outlinePx    描边宽度 (像素单位，建议 1-2px)
     */
    static void DrawOutlinedText(BLContext& ctx, const BLFont& font,
                                 const char* text,
                                 float x, float y,
                                 BLRgba32 fillColor,
                                 BLRgba32 outlineColor,
                                 int outlinePx = 1,
                                 float fontSize = 25);

        /**
         * @brief 绘制带发光效果的射线
         * @param ctx        Blend2D 上下文
         * @param start      起点坐标 (x, y)
         * @param end        终点坐标 (x, y)
         * @param color      主线颜色
         * @param thickness  主线宽度 (默认 1.5f)
         * @param glowAlpha  发光层的透明度 (0-255, 默认 30)
         */
        static void DrawGlowLine(BLContext& ctx,
                                 const BLPoint& start,
                                 const BLPoint& end,
                                 BLRgba32 color,
                                 float thickness = 1.5f,
                                 uint32_t glowAlpha = 30);

    /**
    * @brief 绘制标准射线（单层直线）
    * * @param ctx        Blend2D 绘制上下文
    * @param start      起点坐标 (BLPoint)
    * @param end        终点坐标 (BLPoint)
    * @param color      线条颜色 (BLRgba32)
    * @param thickness  线条宽度 (默认 1.5f)
    * @param roundCap   是否使用圆头线端 (开启后视觉更平滑，默认开启)
    */
    static void DrawLine(BLContext& ctx,
                               const BLPoint& start,
                               const BLPoint& end,
                               BLRgba32 color,
                               float thickness = 1.5f,
                               bool roundCap = true);

    static void DrawRect(BLContext& ctx, float x, float y, float w, float h,
                         BLRgba32 color, float thickness = 1.0f);

    static void DrawRectFilled(BLContext& ctx, float x, float y, float w, float h,
                               BLRgba32 color);

    static void DrawCircle(BLContext& ctx, float centerX, float centerY, float radius,
                           BLRgba32 color, float thickness = 1.0f);

    static void DrawCircleFilled(BLContext& ctx, float centerX, float centerY, float radius,
                                 BLRgba32 color);

    static void DrawHealthBar(BLContext& bctx, float x, float y, float health, float maxHealth);

    static void DrawFancyBox(BLContext& bctx, float x, float y, float w, float h, BLRgba32 color, float healthPercent);
};


#endif //CPURENDERTEMPLATE_DRAWMANAGER_H
