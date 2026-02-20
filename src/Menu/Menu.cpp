//
// Created by Administrator on 2026/2/21.
//

#include "Menu.h"

#include <TouchHelperA.h>
#include "Config/UserConfig.h"

#define ELITE_CHECK(ctx, label, b) \
    do { \
        nk_bool _v = (nk_bool)*(b); \
        nk_checkbox_label(ctx, label, &_v); \
        *(b) = (int)_v; \
    } while (0)

#define ELITE_SLIDER(ctx, label, v, min, max, step, fmt) \
    do { \
        nk_layout_row_dynamic(ctx, 30, 2); \
        nk_label(ctx, label, NK_TEXT_LEFT); \
        char _b[64]; snprintf(_b, sizeof(_b), fmt, (float)*(v)); \
        nk_label_colored(ctx, _b, NK_TEXT_RIGHT, nk_rgb(255, 165, 0)); \
        nk_layout_row_dynamic(ctx, 38, 1); \
        float _val = (float)*(v); \
        if (nk_slider_float(ctx, (float)min, &_val, (float)max, (float)step)) { \
            *(v) = (__typeof__(*(v)))_val; \
        } \
    } while (0)

Menu& Menu::GetInstance() {
    static Menu instance;
    return instance;
}

void Menu::SetupStyle() {
    struct nk_style *s = &m_Ctx.style;
    nk_style_default(&m_Ctx);
    struct nk_color neon = nk_rgb(0, 200, 255);
    struct nk_color neon_accent = nk_rgb(0, 150, 200);

    s->window.background = nk_rgba(15, 25, 35, 255);
    s->window.fixed_background = nk_style_item_color(nk_rgba(15, 25, 35, 255));
    s->window.border_color = neon;
    s->window.border = 2.0f;
    s->window.rounding = 8.0f;
    s->window.padding = nk_vec2(30, 30);
    s->button.normal = nk_style_item_color(nk_rgb(25, 45, 60));
    s->button.hover = nk_style_item_color(nk_rgb(35, 65, 85));
    s->button.active = nk_style_item_color(neon_accent);
    s->button.rounding = 6.0f;
    s->button.text_normal = nk_rgb(180, 220, 255);
    s->button.text_alignment = NK_TEXT_CENTERED;
    s->slider.bar_filled = neon;
    s->slider.cursor_size = nk_vec2(20, 20);
    s->checkbox.cursor_normal = nk_style_item_color(neon);
    s->checkbox.text_normal = nk_rgb(160, 200, 230);
    s->checkbox.text_active = neon;
}

void Menu::Init(BLFont& font) {
    m_Ctx = nk_blend2d_init(font);
    SetupStyle();
}

void Menu::HandleInput() {
    nk_input_begin(&m_Ctx);
    Touch::UpdateNuklearInput(&m_Ctx);
    nk_input_end(&m_Ctx);
}

void Menu::Draw(int screen_width, int screen_height) {
    if (!m_ShowMenu) return;

    float f_w = 1050, f_h = 750;
    struct nk_rect win_r = nk_rect((static_cast<float>(screen_width) - f_w)/2, (static_cast<float>(screen_height) - f_h)/2, f_w, f_h);

    if (nk_begin(&m_Ctx, "CPU-Render", win_r, NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER | NK_WINDOW_MOVABLE)) {

        nk_layout_row_dynamic(&m_Ctx, 50, 1);
        nk_label_colored(&m_Ctx, "CPU-Render", NK_TEXT_CENTERED, nk_rgb(0, 200, 255));
        nk_layout_row_dynamic(&m_Ctx, 8, 1); nk_spacer(&m_Ctx);

        nk_layout_row_dynamic(&m_Ctx, 280, 2);

        RenderVisualCard();
        RenderAimCard();
        RenderSettingsCard();

    }
    nk_end(&m_Ctx);
}

void Menu::Render(BLContext& bctx) {
    if (!m_ShowMenu) return;
    nk_blend2d_render(&m_Ctx, bctx);
    nk_clear(&m_Ctx);
}

void Menu::RenderVisualCard() {
    if (nk_group_begin(&m_Ctx, "card_visual", NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER)) {
        nk_layout_row_dynamic(&m_Ctx, 40, 1);
        nk_label_colored(&m_Ctx, "视觉显示", NK_TEXT_LEFT, nk_rgb(0, 200, 255));
        nk_layout_row_dynamic(&m_Ctx, 8, 1); nk_spacer(&m_Ctx);

        nk_layout_row_dynamic(&m_Ctx, 38, 2);
        if (nk_button_label(&m_Ctx, "获取数据")){
            //获取数据
        }

        if (nk_button_label(&m_Ctx, "退出窗口"))
            exit(0);

        if (nk_button_label(&m_Ctx, "防录屏"))
            g_Config.permeate_record = !g_Config.permeate_record;

        nk_layout_row_dynamic(&m_Ctx, 12, 1); nk_spacer(&m_Ctx);

        nk_layout_row_dynamic(&m_Ctx, 38, 3);
        ELITE_CHECK(&m_Ctx, "方框", &g_Config.visual.box);
        ELITE_CHECK(&m_Ctx, "骨骼", &g_Config.visual.skeleton);
        ELITE_CHECK(&m_Ctx, "射线", &g_Config.visual.line);
        nk_layout_row_dynamic(&m_Ctx, 38, 3);
        ELITE_CHECK(&m_Ctx, "距离", &g_Config.visual.distance);
        ELITE_CHECK(&m_Ctx, "血量", &g_Config.visual.health);
        ELITE_CHECK(&m_Ctx, "名字", &g_Config.visual.name);
        nk_group_end(&m_Ctx);
    }
}

void Menu::RenderAimCard() {
    if (nk_group_begin(&m_Ctx, "card_aim", NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER)) {
        nk_layout_row_dynamic(&m_Ctx, 40, 1);
        nk_label_colored(&m_Ctx, "自瞄协议", NK_TEXT_LEFT, nk_rgb(0, 200, 255));
        nk_layout_row_dynamic(&m_Ctx, 8, 1); nk_spacer(&m_Ctx);

        nk_layout_row_dynamic(&m_Ctx, 38, 2);
        ELITE_CHECK(&m_Ctx, "开启自瞄", &g_Config.aim.enabled);
        ELITE_CHECK(&m_Ctx, "绘制范围", &g_Config.aim.aimcircle);
        nk_layout_row_dynamic(&m_Ctx, 38, 1);
        ELITE_CHECK(&m_Ctx, "绘制自瞄线", &g_Config.aim.aimline);

        nk_layout_row_dynamic(&m_Ctx, 12, 1); nk_spacer(&m_Ctx);

        static const char* m_items[] = {"准星", "距离"};
        static const char* p_items[] = {"头", "胸", "腰"};

        nk_layout_row_dynamic(&m_Ctx, 38, 2);
        nk_label(&m_Ctx, "自瞄逻辑:", NK_TEXT_LEFT);
        g_Config.aim.mod = nk_combo(&m_Ctx, m_items, 2, g_Config.aim.mod, 40, nk_vec2(280, 250));
        nk_layout_row_dynamic(&m_Ctx, 38, 2);
        nk_label(&m_Ctx, "锁定部位:", NK_TEXT_LEFT);
        g_Config.aim.part = nk_combo(&m_Ctx, p_items, 3, g_Config.aim.part, 40, nk_vec2(280, 250));
        nk_group_end(&m_Ctx);
    }
}

void Menu::RenderSettingsCard() {
    if (nk_group_begin(&m_Ctx, "card_settings", NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER)) {
        nk_layout_row_dynamic(&m_Ctx, 40, 1);
        nk_label_colored(&m_Ctx, "系统设置", NK_TEXT_LEFT, nk_rgb(0, 200, 255));
        nk_layout_row_dynamic(&m_Ctx, 8, 1); nk_spacer(&m_Ctx);

        ELITE_SLIDER(&m_Ctx, "自瞄圈范围:", &g_Config.aim.range, 0, 500, 1, "%.0f");
        ELITE_SLIDER(&m_Ctx, "算法预判:", &g_Config.aim.prejudgment, 0, 500, 0.1, "%.1f");
        ELITE_SLIDER(&m_Ctx, "锁定速度:", &g_Config.aim.aimspeed, 0, 50, 0.1, "%.1f");

        nk_group_end(&m_Ctx);
    }
}
