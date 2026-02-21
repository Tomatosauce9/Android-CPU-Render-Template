//
// Created by Administrator on 2026/2/21.
//

#include "DrawManager.h"

/**
 * @param fontSize  新增加的参数：字体大小
 */
void DrawManager::DrawOutlinedText(BLContext& ctx, const BLFont& font,
                                   const char* text,
                                   float x, float y,
                                   BLRgba32 fillColor,
                                   BLRgba32 outlineColor,
                                   int outlinePx,
                                   float fontSize)
{
    BLFont scaledFont(font);
    scaledFont.set_size(fontSize);

    BLGlyphBuffer gb;
    BLTextMetrics m;

    gb.set_utf8_text(text, strlen(text));
    scaledFont.shape(gb);
    scaledFont.get_text_metrics(gb, m);

    float textW = m.bounding_box.x1 - m.bounding_box.x0;
    float textX = x - textW * 0.5f;

    ctx.set_fill_style(outlineColor);
    for (int dx = -outlinePx; dx <= outlinePx; ++dx) {
        for (int dy = -outlinePx; dy <= outlinePx; ++dy) {
            if (dx != 0 || dy != 0)
                ctx.fill_glyph_run(BLPoint(textX + dx, y + dy), scaledFont, gb.glyph_run());
        }
    }

    ctx.set_fill_style(fillColor);
    ctx.fill_glyph_run(BLPoint(textX, y), scaledFont, gb.glyph_run());
}

void DrawManager::DrawGlowLine(BLContext& ctx,
                  const BLPoint& start,
                  const BLPoint& end,
                  BLRgba32 color,
                  float thickness,
                  uint32_t glowAlpha)
{
    BLRgba32 glowColor(color.r(), color.g(), color.b(), glowAlpha);

    ctx.set_stroke_width(thickness * 2.0f);
    ctx.set_stroke_style(glowColor);
    ctx.stroke_line(start.x, start.y, end.x, end.y);

    ctx.set_stroke_width(thickness);
    ctx.set_stroke_style(color);
    ctx.stroke_line(start.x, start.y, end.x, end.y);
}

/**
 * @brief 绘制标准射线（单层直线）
 * * @param ctx        Blend2D 绘制上下文
 * @param start      起点坐标 (BLPoint)
 * @param end        终点坐标 (BLPoint)
 * @param color      线条颜色 (BLRgba32)
 * @param thickness  线条宽度 (默认 1.5f)
 * @param roundCap   是否使用圆头线端 (开启后视觉更平滑，默认开启)
 */
void DrawManager::DrawLine(BLContext& ctx,
                           const BLPoint& start,
                           const BLPoint& end,
                           BLRgba32 color,
                           float thickness,
                           bool roundCap)
{
    ctx.set_stroke_style(color);

    ctx.set_stroke_width(thickness);

    if (roundCap) {
        ctx.set_stroke_caps(BL_STROKE_CAP_ROUND);
    } else {
        ctx.set_stroke_caps(BL_STROKE_CAP_BUTT);
    }

    ctx.stroke_line(start.x, start.y, end.x, end.y);
}

void DrawManager::DrawRect(BLContext& ctx, float x, float y, float w, float h,
                     BLRgba32 color, float thickness)
{
    ctx.set_stroke_style(color);
    ctx.set_stroke_width(thickness);
    ctx.stroke_rect(x, y, w, h);
}

void DrawManager::DrawRectFilled(BLContext& ctx, float x, float y, float w, float h,
                           BLRgba32 color)
{
    ctx.set_fill_style(color);
    ctx.fill_rect(x, y, w, h);
}

void DrawManager::DrawCircle(BLContext& ctx, float centerX, float centerY, float radius,
                       BLRgba32 color, float thickness)
{
    ctx.set_stroke_style(color);
    ctx.set_stroke_width(thickness);
    ctx.stroke_circle(centerX, centerY, radius);
}

void DrawManager::DrawCircleFilled(BLContext& ctx, float centerX, float centerY, float radius,
                             BLRgba32 color)
{
    ctx.set_fill_style(color);
    ctx.fill_circle(centerX, centerY, radius);
}

void DrawManager::DrawHealthBar(BLContext& bctx, float x, float y, float health, float maxHealth) {
    health = (health < 0) ? 0 : (health > maxHealth) ? maxHealth : health;
    float ratio = health / maxHealth;

    const float outerRadius = 28.0f;
    const float innerRadius = 22.0f;
//    const float thickness = outerRadius - innerRadius;
    const int segments = 60;
    const float startAngle = -90.0f;
    const float totalAngle = 360.0f;
    const float endAngle = startAngle + totalAngle * ratio;

    uint8_t red = static_cast<uint8_t>((1.0f - ratio) * 255);
    uint8_t green = static_cast<uint8_t>(ratio * 255);

    BLRgba32 glowColor(0, green, red, 80);
    BLRgba32 mainColor(0, green, red, 255);
    BLRgba32 bgColor(30, 30, 40, 200);
    BLRgba32 coreColor(0, 200, 255, 150);

    bctx.set_fill_style(bgColor);
    bctx.fill_circle(x, y, outerRadius);

    if (ratio > 0.01f) {
        bctx.set_fill_style(glowColor);
        for (int i = 0; i < segments; ++i) {
            float angle1 = startAngle + (i / (float)segments) * (endAngle - startAngle);
            float angle2 = startAngle + ((i + 1) / (float)segments) * (endAngle - startAngle);

            float rad1 = angle1 * 3.14159265f / 180.0f;
            float rad2 = angle2 * 3.14159265f / 180.0f;

            BLPoint points[4];
            points[0] = BLPoint(x + cos(rad1) * innerRadius, y + sin(rad1) * innerRadius);
            points[1] = BLPoint(x + cos(rad2) * innerRadius, y + sin(rad2) * innerRadius);
            points[2] = BLPoint(x + cos(rad2) * (outerRadius + 4), y + sin(rad2) * (outerRadius + 4));
            points[3] = BLPoint(x + cos(rad1) * (outerRadius + 4), y + sin(rad1) * (outerRadius + 4));

            bctx.fill_polygon(points, 4);
        }

        bctx.set_fill_style(mainColor);
        for (int i = 0; i < segments; ++i) {
            float angle1 = startAngle + (i / (float)segments) * (endAngle - startAngle);
            float angle2 = startAngle + ((i + 1) / (float)segments) * (endAngle - startAngle);

            float rad1 = angle1 * 3.14159265f / 180.0f;
            float rad2 = angle2 * 3.14159265f / 180.0f;

            BLPoint points[4];
            points[0] = BLPoint(x + cos(rad1) * innerRadius, y + sin(rad1) * innerRadius);
            points[1] = BLPoint(x + cos(rad2) * innerRadius, y + sin(rad2) * innerRadius);
            points[2] = BLPoint(x + cos(rad2) * outerRadius, y + sin(rad2) * outerRadius);
            points[3] = BLPoint(x + cos(rad1) * outerRadius, y + sin(rad1) * outerRadius);

            bctx.fill_polygon(points, 4);
        }

        bctx.set_fill_style(coreColor);
        for (int i = 0; i < segments; ++i) {
            float angle1 = startAngle + (i / (float)segments) * (endAngle - startAngle);
            float angle2 = startAngle + ((i + 1) / (float)segments) * (endAngle - startAngle);

            float rad1 = angle1 * 3.14159265f / 180.0f;
            float rad2 = angle2 * 3.14159265f / 180.0f;

            BLPoint points[4];
            points[0] = BLPoint(x + cos(rad1) * (innerRadius + 2), y + sin(rad1) * (innerRadius + 2));
            points[1] = BLPoint(x + cos(rad2) * (innerRadius + 2), y + sin(rad2) * (innerRadius + 2));
            points[2] = BLPoint(x + cos(rad2) * (outerRadius - 2), y + sin(rad2) * (outerRadius - 2));
            points[3] = BLPoint(x + cos(rad1) * (outerRadius - 2), y + sin(rad1) * (outerRadius - 2));

            bctx.fill_polygon(points, 4);
        }
    }

    bctx.set_stroke_style(BLRgba32(0, 200, 255, 100));
    bctx.set_stroke_width(1.5f);
    bctx.stroke_circle(x, y, innerRadius);
    bctx.stroke_circle(x, y, outerRadius);

    bctx.set_fill_style(BLRgba32(0, 0, 0, 180));
    bctx.fill_circle(x, y, innerRadius - 2);
}

void DrawManager::DrawFancyBox(BLContext &bctx, float x, float y, float w, float h, BLRgba32 color, float healthPercent) {
    // 外发光效果
    for (int i = 3; i >= 0; i--) {
        float alpha = 50 + i * 30;
        BLRgba32 glowColor = BLRgba32(color.r(), color.g(), color.b(), (uint8_t)alpha);
        bctx.set_stroke_style(glowColor);
        bctx.set_stroke_width(1.0f + i * 0.5f);
        bctx.stroke_rect(x - i*0.5f, y - i*0.5f, w + i, h + i);
    }

    // 主边框
    bctx.set_stroke_style(color);
    bctx.set_stroke_width(2.0f);
    bctx.stroke_rect(x, y, w, h);

    // 内发光
    bctx.set_stroke_style(BLRgba32(255, 255, 255, 100));
    bctx.set_stroke_width(1.0f);
    bctx.stroke_rect(x + 1, y + 1, w - 2, h - 2);

    // 血量条
    if (healthPercent < 0.99f) {
        float healthBarWidth = w - 4;
        float healthBarX = x + 2;
        float healthBarY = y - 8;
        float healthBarHeight = 4;

        // 血条背景
        bctx.set_fill_style(BLRgba32(0, 0, 0, 180));
        bctx.fill_round_rect(healthBarX, healthBarY, healthBarWidth, healthBarHeight, 2.0f);

        // 血条颜色
        BLRgba32 healthColor;
        if (healthPercent > 0.7f) {
            healthColor = BLRgba32(0, 255, 0, 255);
        } else if (healthPercent > 0.3f) {
            healthColor = BLRgba32(255, 165, 0, 255);
        } else {
            healthColor = BLRgba32(255, 0, 0, 255);
        }

        bctx.set_fill_style(healthColor);
        bctx.fill_round_rect(healthBarX, healthBarY, healthBarWidth * healthPercent, healthBarHeight, 2.0f);

        // 血条边框
        bctx.set_stroke_style(BLRgba32(255, 255, 255, 80));
        bctx.set_stroke_width(0.8f);
        bctx.stroke_round_rect(healthBarX, healthBarY, healthBarWidth, healthBarHeight, 2.0f);
    }
}
