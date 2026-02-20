//
// Created by Administrator on 2026/2/21.
//

#ifndef CPURENDERTEMPLATE_USERCONFIG_H
#define CPURENDERTEMPLATE_USERCONFIG_H

#include <string>

struct VisualConfig {
    bool box = false;
    bool skeleton = false;
    bool line = false;
    bool distance = false;
    bool health = false;
    bool name = false;
};

struct AimConfig {
    bool enabled = false;
    bool aimcircle = false;
    bool aimline = false;
    float range = 150.0f;
    float prejudgment = 150.0f;
    float aimspeed = 17.0f;
    int mod = 0;
    int part = 0;
};

struct UserConfig {
    VisualConfig visual;
    AimConfig aim;
    bool renderloop = true;
    bool permeate_record = false;
};

extern UserConfig g_Config;


#endif //CPURENDERTEMPLATE_USERCONFIG_H
