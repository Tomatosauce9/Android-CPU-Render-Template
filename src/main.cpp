
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
        for (int i = 0; i < eventcount; i++) {
            memset(&event, 0, sizeof(input_event));
            read(volumedevicefilearray[i], &event, sizeof(event));
            if (event.type == EV_KEY && event.value == 1) {
                auto& menu = Menu::GetInstance();
                if (event.code == KEY_VOLUMEUP) {
                    menu.Toggle();
                } else if (event.code == KEY_VOLUMEDOWN) {
                    menu.Toggle();
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::microseconds(500));
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

    auto* get_menu_stateth = new std::thread(&get_menu_state);
    get_menu_stateth->detach();

    while (g_Config.renderloop) {
        auto now = std::chrono::steady_clock::now();
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

            std::cout << "分辨率更新: " << screenState.screen_width << "x" << screenState.screen_height << std::endl;
        }

        if (!g_Config.permeate_record) {
            android::ANativeWindowCreator::ProcessMirrorDisplay();
        }

        menu.HandleInput();

        BLContext& bctx = renderer.GetContext();
        bctx.clear_all();

        menu.Draw(renderer.GetWidth(), renderer.GetHeight());
        menu.Render(bctx);

        renderer.Present();
    }
    delete get_menu_stateth;

    return 0;

}