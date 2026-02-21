# Android-CPU-Render-Template 🚀

[English](./README.md) | [Chinese](./README-cn.md)

## Introduction

A high-performance Android CPU rendering template based on Blend2D and Nuklear UI. Designed for developers who need a lightweight, efficient, and modular starting point for native Android graphics applications.

## Supported Android Versions

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

## Requirements

### Runtime Environment
- **Android Device**: Android 5.0 (Lollipop) or higher.
- **Permissions**: Requires System-level or Root privileges for Surface access.

## Quick Start

### 1.Clone the Repository

```bash
git clone https://github.com/Tomatosauce9/Android-CPU-Render-Template.git
cd Android-CPU-Render-Template
```

### 2. Build the Project

Before building, ensure your NDK environment variable is set (or modify the path in the command below):

```bash
#Set NDK path (Windows PowerShell example; use 'export' for Linux)
$env:NDK_ROOT = "C:/Your/Path/To/android-ndk-r27c"

#Create and enter build directory
mkdir build && cd build

#Configure CMake project
cmake .. -G "Ninja" \
    -DCMAKE_TOOLCHAIN_FILE="$env:NDK_ROOT/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_NATIVE_API_LEVEL=21 \
    -DCMAKE_BUILD_TYPE=Release

#Execute build
cmake --build .
```

### 3. Deployment & Execution

After a successful build, push the binary to your device and set the necessary permissions:

```bash
# Push the binary to the device
adb push outputs/arm64-v8a/CPURenderTemplate /data/local/tmp/

# Grant execute permission
adb shell "chmod +x /data/local/tmp/CPURenderTemplate"

# Execute the binary (requires root)
adb shell su -c "/data/local/tmp/CPURenderTemplate"
```

## Project Structure

```
Android-CPU-Render-Template/
├── src/                      # Core Source Code
│   ├── AndroidRender/        # Rendering: Handles ANativeWindow & Pixel buffer submission
│   ├── Config/               # Config System: Binary Save/Load & Global state management
│   └── Menu/                 # UI Logic: Immediate-mode menu layouts via Nuklear
├── third_party/              # External Dependencies
│   ├── blend2d/              # High-performance 2D Vector Graphics Engine
│   ├── native_surface/       # Native Window Creator/Bridge
│   ├── nuklear/              # Lightweight IMGUI Framework
│   └── touch/                # Android Touch Event Parsing Module
├── outputs/                  # Build Outputs (Binaries)
├── CMakeLists.txt            # CMake Configuration
├── README.md                 # Project Documentation (English)
└── README-cn.md              # Project Documentation (Chinese)
```

## Contribution Guide

Issues and Pull Requests are always welcome!

1. Fork the Project. 
2. Create your Feature Branch (git checkout -b feature/AmazingFeature). 
3. Commit your changes (git commit -m 'Add some AmazingFeature'). 
4. Push to the Branch (git push origin feature/AmazingFeature).
5. Open a Pull Request.

## License

This project is licensed under the [MIT License](./LICENSE).

## Acknowledgements

- **[AFan4724/AndroidSurfaceImgui-Enhanced](https://github.com/AFan4724/AndroidSurfaceImgui-Enhanced)** - Thanks for providing core source code inspiration.
- **[Blend2D](https://github.com/blend2d/blend2d)** - For the high-performance 2D vector rendering core.
- **[Nuklear](https://github.com/Immediate-Mode-UI/Nuklear)** - For the lightweight immediate-mode UI solution.
- **All Contributors & Testers** - Thank you for your support and feedback!