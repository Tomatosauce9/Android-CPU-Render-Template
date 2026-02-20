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
    m_Width = width;
    m_Height = height;

    ANativeWindow_setBuffersGeometry(window, width, height, WINDOW_FORMAT_RGBA_8888);

    BLResult result = m_Canvas.create(width, height, BL_FORMAT_XRGB32);
    if (result != BL_SUCCESS) {
        std::cout << "创建 Blend2D Image 失败: " << result << "\n";
        return false;
    }

    BLContextCreateInfo createInfo{};
    createInfo.thread_count = 4;

    m_Ctx.end();
    m_Ctx.begin(m_Canvas, createInfo);

    m_Ctx.set_comp_op(BL_COMP_OP_SRC_OVER);
    m_Ctx.set_stroke_width(1.0);

    return true;
}

void AndroidRender::Recreate(const android::ANativeWindowCreator::DisplayInfo& info, bool permeate, Menu& menu, BLFont& font) {
    std::lock_guard<std::mutex> lock(m_RenderMutex);

    if (m_Window) {
        android::ANativeWindowCreator::Destroy(m_Window);
    }
    m_Ctx.end();
    m_Canvas.reset();

    m_Width = info.width;
    m_Height = info.height;
    m_Window = android::ANativeWindowCreator::Create("Nuklear", m_Width, m_Height, permeate);
    ANativeWindow_setBuffersGeometry(m_Window, m_Width, m_Height, WINDOW_FORMAT_RGBA_8888);

    m_Canvas.create(m_Width, m_Height, BL_FORMAT_XRGB32);
    BLContextCreateInfo createInfo{};
    createInfo.thread_count = 4;
    m_Ctx.begin(m_Canvas, createInfo);

    menu.Init(font);
}

void AndroidRender::Present() {
    if (!m_Window || !m_Canvas._impl()) return;

    std::lock_guard<std::mutex> lock(m_RenderMutex);

    m_Ctx.flush(BL_CONTEXT_FLUSH_SYNC);

    ANativeWindow_Buffer buffer;
    if (ANativeWindow_lock(m_Window, &buffer, nullptr) != 0) {
        std::cout << "无法锁定window\n";
        return;
    }

    BLImageData data{};
    m_Canvas.get_data(&data);

    auto* src = (uint8_t*)data.pixel_data;
    auto* dst = (uint8_t*)buffer.bits;

    int copyWidthBytes = std::min(m_Width, buffer.width) * 4;
    int copyHeight = std::min(m_Height, buffer.height);
    int srcStride = static_cast<int>(data.stride);
    int dstStride = buffer.stride * 4;

#pragma omp parallel for if(copyHeight > 200)
    for (int y = 0; y < copyHeight; y++) {
        std::memcpy(dst + y * dstStride, src + y * srcStride, copyWidthBytes);
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