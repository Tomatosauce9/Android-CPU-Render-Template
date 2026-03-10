
#include <iostream>
#include <thread>
#include <asm-generic/fcntl.h>
#include <fcntl.h>
#include <unistd.h>

#include <ANativeWindowCreator.h>
#include <TouchHelperA.h>
#include <dirent.h>

#include "AndroidRender/AndroidRender.h"
#include "Menu/Menu.h"
#include "Config/UserConfig.h"
#include "DrawManager/DrawManager.h"

bool readTouchEvent = true;

struct ScreenState {
    uint32_t current_orientation = 0;
    int screen_width = 0;
    int screen_height = 0;
    int permeate_record = 0;

    void update(const android::ANativeWindowCreator::DisplayInfo& info) {
        current_orientation = info.orientation;

        screen_width = info.width;
        screen_height = info.height;

        Touch::setOrientation(info.orientation);
    }
};

int GetEventCount() {
    int count = 0;
    dirent *ptr = nullptr;
    DIR *dir = opendir("/dev/input/");
    while ((ptr = readdir(dir)) != nullptr) {
        if (strstr(ptr->d_name, "event")) {
            count++;
        }
    }
    return count ? count : -1;
}

void get_menu_state() {
    input_event event;
    int eventcount = GetEventCount();
    if (eventcount <= 0) {
        std::cout << "\033[31;1m[-] 获取音量监听失败\033[30;1m" << std::endl;
        return;
    }
    int *volumedevicefilearray = (int *) malloc(eventcount * sizeof(int));
    for (int i = 0; i < eventcount; i++) {
        char inputfilepath[128] = "";
        sprintf(inputfilepath, "/dev/input/event%d", i);
        volumedevicefilearray[i] = open(inputfilepath, O_RDWR | O_NONBLOCK);
    }
    for (;;) {
        bool key_detected = false;
        for (int i = 0; i < eventcount; i++) {
            if (read(volumedevicefilearray[i], &event, sizeof(event)) > 0) {
                if (event.type == EV_KEY && event.value == 1) {
                    if (event.code == KEY_VOLUMEUP || event.code == KEY_VOLUMEDOWN) {
                        Menu::GetInstance().Toggle();
                        key_detected = true;
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

}

int main() {
    auto display = android::ANativeWindowCreator::GetDisplayInfo();
    if (display.width <= 0 || display.height <= 0) return -1;

    auto& renderer = AndroidRender::GetInstance();
    auto& menu = Menu::GetInstance();
    BLFont font = nk_zh_font_init();

    auto window = android::ANativeWindowCreator::Create("Nuklear", display.width, display.height, g_Config.permeate_record);
    Touch::Init({(float)display.width, (float)display.height}, readTouchEvent);
    Touch::setOrientation(display.orientation);

    menu.Init(font);
    renderer.Init(window, display.width, display.height);

    ScreenState screenState;
    screenState.update(display);
    screenState.permeate_record = g_Config.permeate_record;

    auto lastOrientationChange = std::chrono::steady_clock::now();

    std::thread menuThread([&]() {
        get_menu_state();
    });
    menuThread.detach();

    const int targetFPS = 120;
    const std::chrono::nanoseconds frameTargetTime(1000000000 / targetFPS);
    static char info[64] = "";
    static auto lastFpsUpdate = std::chrono::steady_clock::now();

    while (g_Config.renderloop) {

        auto frameStart = std::chrono::high_resolution_clock::now();
        auto now = std::chrono::steady_clock::now();

        static int checkCounter = 0;
        if (checkCounter++ % 30 == 0) {
            auto newDisplayInfo = android::ANativeWindowCreator::GetDisplayInfo();

            bool isChanged = (newDisplayInfo.orientation != screenState.current_orientation ||
                              newDisplayInfo.width != screenState.screen_width ||
                              newDisplayInfo.height != screenState.screen_height || g_Config.permeate_record != screenState.permeate_record);

            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastOrientationChange).count();

            if (isChanged && elapsed > 500) {
                lastOrientationChange = now;

                screenState.update(newDisplayInfo);
                screenState.permeate_record = g_Config.permeate_record;
                renderer.Recreate(newDisplayInfo, g_Config.permeate_record, menu, font);

                std::cout << "分辨率更新: " << screenState.screen_width << "x" << screenState.screen_height  << std::endl;
                std::cout << "防录屏更新: " << (g_Config.permeate_record ? "开启" : "关闭") << std::endl;
            }
        }

        if (!g_Config.permeate_record) {
            android::ANativeWindowCreator::ProcessMirrorDisplay();
        }

        menu.HandleInput();

        BLContext& bctx = renderer.GetContext();
        bctx.clear_all();

        if (menu.IsVisible()) {
            menu.Draw(renderer.GetWidth(), renderer.GetHeight());
            menu.Render(bctx);
        }

        BLPoint pX = {0,0};
        BLPoint pY = {static_cast<double>(screenState.screen_width),static_cast<double>(screenState.screen_height)};

        BLPoint p2X = {static_cast<double>(screenState.screen_width),0};
        BLPoint p2Y = {0,static_cast<double>(screenState.screen_height)};

        DrawManager::DrawOutlinedText(bctx, font, "TEST1", 200, 200, BLRgba32(255,0,0,255), BLRgba32(0xFF000000));
        DrawManager::DrawOutlinedText(bctx, font, "TEST2", 200, 250, BLRgba32(0,255,0,255), BLRgba32(0xFF000000));

        DrawManager::DrawLine(bctx, pX, pY, BLRgba32(0,255,0,255), 1.5f, true);
        DrawManager::DrawLine(bctx, p2X, p2Y, BLRgba32(255,0,0,255), 1.5f, 30);

        DrawManager::DrawRect(bctx, 200, 275, 100, 100, BLRgba32(255, 0, 0, 255), 1.5f);
        DrawManager::DrawRectFilled(bctx, 200, 400, 100, 100, BLRgba32(255, 0, 0, 255));

        DrawManager::DrawCircle(bctx, 200, 550, 25, BLRgba32(255, 0, 0, 255), 1);
        DrawManager::DrawCircleFilled(bctx, 200, 625, 25, BLRgba32(255, 0, 0, 255));

        DrawManager::DrawHealthBar(bctx, 200, 700, 100, 100);
        DrawManager::DrawHealthBar(bctx, 200, 775, 50, 100);

        DrawManager::DrawFancyBox(bctx, 200, 875, 100, 100, BLRgba32(255, 0, 0, 255), 100.f/100.f);
        DrawManager::DrawFancyBox(bctx, 350, 875, 100, 100, BLRgba32(255, 0, 0, 255), 50.f/100.f);

        DrawManager::DrawOutlinedText(bctx, font, info, 200, 50,
                                      BLRgba32(0xFF0000FF), BLRgba32(0xFF000000));

        renderer.Present();

        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto workDuration = frameEnd - frameStart;

        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFpsUpdate).count() > 500) {
            double ms = std::chrono::duration<double, std::milli>(workDuration).count();
            snprintf(info, sizeof(info), "FPS: %.1f | %.2f ms", 1000.0 / (ms > 1.0 ? ms : 11.0), ms);
            lastFpsUpdate = now;
        }

        if (workDuration < frameTargetTime) {
            std::this_thread::sleep_for(frameTargetTime - workDuration);
        }
    }

    return 0;
}