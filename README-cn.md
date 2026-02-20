# Android-CPU-Render-Template

[English](./README.md) | [Chinese](./README-cn.md)

## 项目介绍

本项目是一个基于 **Blend2D** 与 **Nuklear UI** 的 Android CPU 渲染模版

## 支持的Android版本

| 版本范围 | 支持状态 | 备注 |
|---------|---------|------|
| Android 5.0 - 5.1 | ✅ 完全支持 | API Level 21-22 |
| Android 6.0 | ✅ 完全支持 | API Level 23 |
| Android 7.0 - 7.1 | ✅ 完全支持 | API Level 24-25 |
| Android 8.0 - 8.1 | ✅ 完全支持 | API Level 26-27 |
| Android 9.0 | ✅ 完全支持 | API Level 28 |
| Android 10.0 | ✅ 完全支持 | API Level 29 |
| Android 11.0 | ✅ 完全支持 | API Level 30 |
| Android 12.0 - 12.1 | ✅ 完全支持 | API Level 31-32 |
| Android 13.0 | ✅ 完全支持 | API Level 33 |
| Android 14.0 | ✅ 完全支持 | API Level 34 |
| Android 15.0 | ✅ 完全支持 | API Level 35 |
| Android 16.0 | ✅ 完全支持 | API Level 36 |

## 环境要求

### 运行环境
- **Android设备**: Android 5.0及以上
- **权限要求**: 需要系统级权限或root权限

## 快速开始

### 1. 克隆项目

```bash
git clone https://github.com/Tomatosauce9/Android-CPU-Render-Template.git
cd Android-CPU-Render-Template
```

### 2. 编译项目

在编译前，请确保已设置 NDK 路径环境变量（或直接修改下方指令中的路径）：

```bash
#设置 NDK 路径 (Windows PowerShell 示例，Linux 使用 export)
$env:NDK_ROOT = "C:/Your/Path/To/android-ndk-r27c"

#创建并进入构建目录
mkdir build && cd build

#配置 CMake 项目
cmake .. -G "Ninja" \
    -DCMAKE_TOOLCHAIN_FILE="$env:NDK_ROOT/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_NATIVE_API_LEVEL=21 \
    -DCMAKE_BUILD_TYPE=Release

#执行编译
cmake --build .
```

### 3. 部署与运行

编译完成后，将生成的二进制文件推送到设备并赋予权限：

```bash
#推送文件
adb push outputs/arm64-v8a/CPURenderTemplate /data/local/tmp/

#赋予执行权限并运行
adb shell "chmod +x /data/local/tmp/CPURenderTemplate"

#运行程序
adb shell su -c "/data/local/tmp/CPURenderTemplate"
```

## 项目结构

```
Android-CPU-Render-Template/
├── src/                      #核心源代码
│   ├── AndroidRender/        #渲染
│   ├── Config/               #配置系统
│   └── Menu/                 #UI 逻辑层：基于 Nuklear 的即时模式菜单布局
├── third_party/              #外部依赖库
│   ├── blend2d/              #CPU绘图引擎
│   ├── native_surface/       #窗口创建器
│   ├── nuklear/              #轻量级UI 框架
│   └── touch/                #Android 触控事件解析模块
├── outputs/                  #编译产物
├── CMakeLists.txt            #构建配置文件
└── README.md              #项目说明文档
└── README-cn.md              #项目中文说明文档
```

## 贡献指南

欢迎提交Issue和Pull Request！

1. Fork本项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启Pull Request

## 许可证

本项目采用 [MIT License](./LICENSE) 进行许可

## 致谢

- **[AFan4724/AndroidSurfaceImgui-Enhanced](https://github.com/AFan4724/AndroidSurfaceImgui-Enhanced)** - 感谢提供项目的源码
- **[Blend2D](https://github.com/blend2d/blend2d)** - 提供高性能的 2D 矢量渲染核心
- **[Nuklear](https://github.com/Immediate-Mode-UI/Nuklear)** - 提供轻量的即时模式 UI 解决方案
- **所有贡献者与测试者** - 感谢你们
