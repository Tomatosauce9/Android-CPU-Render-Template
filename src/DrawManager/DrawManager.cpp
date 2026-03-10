//
// Created by Administrator on 2026/2/21.
//

#include "DrawManager.h"

#include <algorithm>

namespace {
    constexpr float PI_2 = 1.570796f;
    constexpr float TWO_PI = 6.283185f;
}

void DrawManager::DrawOutlinedText(BLContext& ctx, const BLFont& font,
                                   const char* text, float x, float y,
                                   BLRgba32 fillColor, BLRgba32 outlineColor) {
    thread_local BLGlyphBuffer gb;
    gb.set_utf8_text(text);
    font.shape(gb);
    const BLGlyphRun& run = gb.glyph_run();

    BLTextMetrics tm;
    font.get_text_metrics(gb, tm);
    float textW = (float)(tm.bounding_box.x1 - tm.bounding_box.x0);

    BLPoint pos(std::floor(x - textW / 2.0f), std::floor(y));

    ctx.set_fill_style(outlineColor);
    ctx.fill_glyph_run(BLPoint(pos.x + 2.0f, pos.y + 2.0f), font, run);

    ctx.set_fill_style(fillColor);
    ctx.fill_glyph_run(pos, font, run);
}

void DrawManager::DrawBatchRays(BLContext& ctx, const std::vector<BLPoint>& targets,
                                BLPoint start, BLRgba32 color, float thickness) {
    if (targets.empty()) return;

    thread_local BLPath rayPath;
    rayPath.clear();

    for (const auto& end : targets) {
        rayPath.move_to(start.x, start.y);
        rayPath.line_to(end.x, end.y);
    }

    ctx.set_stroke_style(color);
    ctx.set_stroke_width(thickness);
    ctx.stroke_path(rayPath);
}

void DrawManager::DrawLine(BLContext& ctx, const BLPoint& start, const BLPoint& end,
                           BLRgba32 color, float thickness, bool roundCap) {
    ctx.set_stroke_style(color);
    ctx.set_stroke_width(thickness);
    ctx.set_stroke_caps(roundCap ? BL_STROKE_CAP_ROUND : BL_STROKE_CAP_BUTT);
    ctx.stroke_line(start.x, start.y, end.x, end.y);
}

void DrawManager::DrawRect(BLContext& ctx, float x, float y, float w, float h,
                           BLRgba32 color, float thickness) {
    ctx.set_stroke_style(color);
    ctx.set_stroke_width(thickness);
    ctx.stroke_rect(std::floor(x), std::floor(y), std::floor(w), std::floor(h));
}

void DrawManager::DrawRectFilled(BLContext& ctx, float x, float y, float w, float h,
                                 BLRgba32 color) {
    ctx.set_fill_style(color);
    ctx.fill_rect(std::floor(x), std::floor(y), std::floor(w), std::floor(h));
}

void DrawManager::DrawCircle(BLContext& ctx, float centerX, float centerY, float radius,
                             BLRgba32 color, float thickness)
{
    float ix = std::floor(centerX);
    float iy = std::floor(centerY);
    float ir = std::floor(radius);

    ctx.set_stroke_style(color);
    ctx.set_stroke_width(thickness);

    ctx.stroke_circle(ix, iy, ir);
}

void DrawManager::DrawCircleFilled(BLContext& ctx, float centerX, float centerY, float radius,
                                   BLRgba32 color)
{
    float ix = std::floor(centerX);
    float iy = std::floor(centerY);
    float ir = std::floor(radius);

    ctx.set_fill_style(color);

    ctx.fill_circle(ix, iy, ir);
}

void DrawManager::DrawHealthBar(BLContext& bctx, float x, float y, float health, float maxHealth) {
    float ratio = std::max(0.0f, std::min(1.0f, health / maxHealth));

    float w = 50.0f;
    float h = 4.0f;
    float ix = std::floor(x - w/2);
    float iy = std::floor(y);

    bctx.set_fill_style(BLRgba32(0, 0, 0, 150));
    bctx.fill_rect(ix, iy, w, h);

    uint8_t r = static_cast<uint8_t>((1.0f - ratio) * 255);
    uint8_t g = static_cast<uint8_t>(ratio * 255);

    bctx.set_fill_style(BLRgba32(r, g, 0, 255));
    bctx.fill_rect(ix, iy, w * ratio, h);

    bctx.set_stroke_style(BLRgba32(255, 255, 255, 100));
    bctx.set_stroke_width(1.0f);
    bctx.stroke_rect(ix, iy, w, h);
}

void DrawManager::DrawFancyBox(BLContext &bctx, float x, float y, float w, float h,
                               BLRgba32 color, float healthPercent) {
    float ix = std::floor(x);
    float iy = std::floor(y);
    float iw = std::floor(w);
    float ih = std::floor(h);

    bctx.set_stroke_style(BLRgba32(color.r(), color.g(), color.b(), 60));
    bctx.set_stroke_width(3.0f);
    bctx.stroke_rect(ix - 1.0f, iy - 1.0f, iw + 2.0f, ih + 2.0f);

    bctx.set_stroke_style(color);
    bctx.set_stroke_width(1.5f);
    bctx.stroke_rect(ix, iy, iw, ih);

    float hbW = 3.0f;
    float hbH = ih;
    float hbX = ix - 6.0f;
    float hbY = iy;

    bctx.set_fill_style(BLRgba32(0, 0, 0, 150));
    bctx.fill_rect(hbX, hbY, hbW, hbH);

    BLRgba32 hColor = (healthPercent > 0.7f) ? BLRgba32(0, 255, 0) :
                      (healthPercent > 0.3f) ? BLRgba32(255, 165, 0) : BLRgba32(255, 0, 0);

    bctx.set_fill_style(hColor);

    float currentH = hbH * healthPercent;
    bctx.fill_rect(hbX, hbY + (hbH - currentH), hbW, currentH);
}