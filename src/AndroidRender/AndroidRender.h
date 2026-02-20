//
// Created by Administrator on 2026/2/21.
//

#ifndef CPURENDERTEMPLATE_ANDROIDRENDER_H
#define CPURENDERTEMPLATE_ANDROIDRENDER_H

#include <mutex>

#include <blend2d.h>
#include <ANativeWindowCreator.h>

struct ANativeWindow;

class Menu;

class AndroidRender {
protected:
    ANativeWindow *m_Window = nullptr;
    int m_Width = 0;
    int m_Height = 0;

    BLImage m_Canvas;
    BLContext m_Ctx;

    std::mutex m_RenderMutex;
public:
    AndroidRender(const AndroidRender&) = delete;
    AndroidRender& operator=(const AndroidRender&) = delete;

    static AndroidRender& GetInstance();

    bool Init(ANativeWindow *window, int width, int height);

    void Recreate(const android::ANativeWindowCreator::DisplayInfo& info, bool permeate, Menu& menu, BLFont& font);

    BLContext& GetContext() { return m_Ctx; }

    void Present();

    void Release();

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    AndroidRender() = default;
    ~AndroidRender();

};


#endif //CPURENDERTEMPLATE_ANDROIDRENDER_H
