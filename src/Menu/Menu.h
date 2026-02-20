//
// Created by Administrator on 2026/2/21.
//

#ifndef CPURENDERTEMPLATE_MENU_H
#define CPURENDERTEMPLATE_MENU_H

#ifndef MENU_H
#define MENU_H

#include <nuklear_blend2d.h>

class Menu {
public:
    Menu(const Menu&) = default;
    Menu& operator=(const Menu&) = delete;

    static Menu& GetInstance();

    void Init(BLFont& font);

    void HandleInput();

    void Draw(int screen_width, int screen_height);

    void Render(BLContext& bctx);

    void Toggle() { m_ShowMenu = !m_ShowMenu; }
    bool IsVisible() const { return m_ShowMenu; }

private:

    void RenderVisualCard();
    void RenderAimCard();
    void RenderSettingsCard();

    void SetupStyle();

private:
    Menu() = default;
    ~Menu()= default;

    struct nk_context m_Ctx{};
    bool m_ShowMenu = true;
};

#endif

#endif //CPURENDERTEMPLATE_MENU_H
