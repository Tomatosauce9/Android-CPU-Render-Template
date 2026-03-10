//
// Created by Administrator on 2026/2/21.
//

#ifndef CPURENDERTEMPLATE_DRAWMANAGER_H
#define CPURENDERTEMPLATE_DRAWMANAGER_H

#include <blend2d.h>
#include <string>
#include <vector>
#include <unordered_map>

struct CachedText {
    BLImage img;
    uint32_t lastFrameUsed;
};

class DrawManager {
public:

    static void DrawOutlinedText(BLContext& ctx, const BLFont& font,
                                 const char* text, float x, float y,
                                 BLRgba32 fillColor, BLRgba32 outlineColor);

    static void DrawBatchRays(BLContext& ctx, const std::vector<BLPoint>& targets,
                              BLPoint start, BLRgba32 color, float thickness);

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
