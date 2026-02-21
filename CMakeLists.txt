cmake_minimum_required(VERSION 3.29)

if (NOT DEFINED CMAKE_ANDROID_NDK)
    set(CMAKE_ANDROID_NDK "C:/AAAA/android-ndk-r29-beta4")
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_ANDROID_NDK}/build/cmake/android.toolchain.cmake")
    set(ANDROID_ABI "arm64-v8a" CACHE STRING "")
    set(ANDROID_PLATFORM "android-26" CACHE STRING "")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

project(CPURenderTemplate LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/outputs/${ANDROID_ABI}")

set(RENDERER_PERF_FLAGS
        "-O3"
        "-ffast-math"
        "-fno-math-errno"
        "-fno-trapping-math"
        "-fdata-sections"
        "-ffunction-sections"
        "-fno-plt"
        "-fno-rtti"
        "-fno-exceptions"
        "-fmerge-all-constants"
        "-fno-semantic-interposition"
)
set(SECURITY_FLAGS "-fstack-protector-strong -D_FORTIFY_SOURCE=2")
set(COMMON_LINKER_FLAGS "-Wl,--gc-sections -Wl,--as-needed -Wl,-z,relro,-z,now")

add_library(blend2d STATIC IMPORTED)
set_target_properties(blend2d PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/third_party/blend2d/lib/libblend2d.a"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/third_party/blend2d/include"
        INTERFACE_COMPILE_DEFINITIONS "BLEND2D_STATIC"
)

add_library(nuklear STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/nuklear/src/nuklear_blend2d.cpp"
)
target_include_directories(nuklear SYSTEM PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/nuklear/include"
)
target_link_libraries(nuklear PRIVATE blend2d)
target_compile_options(nuklear PRIVATE $<$<CONFIG:Release>:${RENDERER_PERF_FLAGS}>)

add_library(native_surface INTERFACE)
target_include_directories(native_surface SYSTEM INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/native_surface/include"
)

add_library(touch STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/touch/src/TouchHelperA.cpp"
)
target_include_directories(touch SYSTEM PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/touch/include"
)
target_link_libraries(touch PRIVATE nuklear)
target_compile_options(touch PRIVATE
        $<$<CONFIG:Release>:-O2 -fdata-sections -ffunction-sections -fno-rtti>
)
target_link_libraries(touch PRIVATE log android)

set(FILE_SOURCES
        "src/main.cpp"
        "src/AndroidRender/AndroidRender.cpp"
        "src/Menu/Menu.cpp"
        "src/Config/UserConfig.cpp"
        "src/DrawManager/DrawManager.cpp"
)

add_executable(${PROJECT_NAME} ${FILE_SOURCES})

if(ANDROID_ABI STREQUAL "arm64-v8a")
    target_compile_options(${PROJECT_NAME} PRIVATE "-march=armv8-a+simd+crypto" "-mno-outline-atomics")
endif()

target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:${RENDERER_PERF_FLAGS} ${SECURITY_FLAGS}>
        $<$<CONFIG:Debug>:-O0 -g -Wall>
)

target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:${COMMON_LINKER_FLAGS} -s>
)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT error)
    if(ipo_supported)
        set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "LTO is not supported: ${error}")
    endif()
endif()

target_include_directories(${PROJECT_NAME} PUBLIC src)

target_link_libraries(${PROJECT_NAME}
        nuklear
        blend2d
        native_surface
        touch
        log
        android
        z
        m
)