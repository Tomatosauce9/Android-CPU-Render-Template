//
// Created by Administrator on 2026/2/21.
//

#include "AndroidRender.h"

#include <iostream>
#include <algorithm>
#include <android/native_window.h>

#include "Menu/Menu.h"

AndroidRender& AndroidRender::GetInstance() {
    static AndroidRender instance;
    return instance;
}

bool AndroidRender::Init(ANativeWindow *window, int width, int height) {
    std::lock_guard<std::mutex> lock(m_RenderMutex);
    m_Window = window;
    m_BaseW = width;
    m_BaseH = height;

    m_RenderW = (int)(width * m_Scale);
    m_RenderH = (int)(height * m_Scale);

    ANativeWindow_setBuffersGeometry(m_Window, m_RenderW, m_RenderH, WINDOW_FORMAT_RGBA_8888);

    m_Canvas.create(m_RenderW, m_RenderH, BL_FORMAT_XRGB32);

    BLContextCreateInfo createInfo{};
    createInfo.thread_count = 4;
    m_Ctx.begin(m_Canvas, createInfo);

    m_Ctx.scale(m_Scale);

    return true;
}

void AndroidRender::Recreate(const android::ANativeWindowCreator::DisplayInfo& info, bool permeate, Menu& menu, BLFont& font) {
    std::lock_guard<std::mutex> lock(m_RenderMutex);

    m_Ctx.end();
    m_Canvas.reset();
    if (m_Window) {
        android::ANativeWindowCreator::Destroy(m_Window);
    }

    m_BaseW = info.width;
    m_BaseH = info.height;

    m_RenderW = (int)(m_BaseW * m_Scale);
    m_RenderH = (int)(m_BaseH * m_Scale);

    m_Window = android::ANativeWindowCreator::Create("Nuklear", m_BaseW, m_BaseH, permeate);
    ANativeWindow_setBuffersGeometry(m_Window, m_RenderW, m_RenderH, WINDOW_FORMAT_RGBA_8888);

    m_Canvas.create(m_RenderW, m_RenderH, BL_FORMAT_XRGB32);

    BLContextCreateInfo createInfo{};
    createInfo.thread_count = 4;
    m_Ctx.begin(m_Canvas, createInfo);

    m_Ctx.scale(m_Scale);

    menu.Init(font);
}

void AndroidRender::Present() {
    std::lock_guard<std::mutex> lock(m_RenderMutex);
    if (!m_Window || m_Canvas.is_empty()) return;

    m_Ctx.flush(BL_CONTEXT_FLUSH_SYNC);

    ANativeWindow_Buffer buffer;
    if (ANativeWindow_lock(m_Window, &buffer, nullptr) != 0) return;

    BLImageData data;
    m_Canvas.get_data(&data);

    uint8_t* dst = (uint8_t*)buffer.bits;
    uint8_t* src = (uint8_t*)data.pixel_data;

    if (buffer.stride * 4 == data.stride) {
        memcpy(dst, src, data.stride * m_RenderH);
    } else {
        for (int y = 0; y < m_RenderH; y++) {
            memcpy(dst + y * buffer.stride * 4,
                   src + y * data.stride,
                   m_RenderW * 4);
        }
    }

    ANativeWindow_unlockAndPost(m_Window);
}

void AndroidRender::Release() {
    std::lock_guard<std::mutex> lock(m_RenderMutex);
    if (m_Ctx._impl()) {
        m_Ctx.end();
    }
    m_Canvas.reset();
    if (m_Window) {
        android::ANativeWindowCreator::Destroy(m_Window);
        m_Window = nullptr;
    }
    android::ANativeWindowCreator::Cleanup();
}

AndroidRender::~AndroidRender() {
    Release();
}