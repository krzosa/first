#pragma once
#ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
#endif

#if defined(__APPLE__) && defined(__MACH__)
    #define OS_MAC 1
#elif defined(_WIN32)
    #define OS_WINDOWS 1
#elif defined(__linux__)
    #define OS_POSIX 1
    #define OS_LINUX 1
#else
    #error Unsupported platform
#endif

#if defined(__clang__)
    #define COMPILER_CLANG 1
#elif defined(__GNUC__) || defined(__GNUG__)
    #define COMPILER_GCC 1
#elif defined(_MSC_VER)
    #define COMPILER_MSVC 1
#elif defined(__TINYC__)
    #define COMPILER_TCC 1
#else
    #error Unsupported compiler
#endif

#ifdef __cplusplus
    #define LANG_CPP 1
#else
    #define LANG_C 1
#endif

#ifndef OS_MAC
    #define OS_MAC 0
#endif

#ifndef OS_WINDOWS
    #define OS_WINDOWS 0
#endif

#ifndef OS_LINUX
    #define OS_LINUX 0
#endif

#ifndef OS_POSIX
    #define OS_POSIX 0
#endif

#ifndef COMPILER_MSVC
    #define COMPILER_MSVC 0
#endif

#ifndef COMPILER_CLANG
    #define COMPILER_CLANG 0
#endif

#ifndef COMPILER_GCC
    #define COMPILER_GCC 0
#endif

#ifndef COMPILER_TCC
    #define COMPILER_TCC 0
#endif

#ifndef LANG_CPP
    #define LANG_CPP 0
#endif

#ifndef LANG_C
    #define LANG_C 0
#endif

#if COMPILER_MSVC
    #define FORCE_INLINE __forceinline
#elif COMPILER_GCC || COMPILER_CLANG
    #define FORCE_INLINE __attribute__((always_inline)) inline
#else
    #define FORCE_INLINE inline
#endif

#if OS_MAC
    #define ON_MAC(x) x
#else
    #define ON_MAC(x)
#endif

#if OS_WINDOWS
    #define ON_WINDOWS(x) x
    #define IF_WINDOWS_ELSE(x, y) x
#else
    #define ON_WINDOWS(x)
    #define IF_WINDOWS_ELSE(x, y) y
#endif

#if OS_LINUX
    #define ON_LINUX(x) x
    #define IF_LINUX_ELSE(x, y) x
#else
    #define ON_LINUX(x)
    #define IF_LINUX_ELSE(x, y) y
#endif

#if OS_WINDOWS
    #define OS_NAME "Windows"
#elif OS_LINUX
    #define OS_NAME "Linux"
#elif OS_MAC
    #define OS_NAME "MacOS"
#else
    #error couldn't figure out OS
#endif

// #if COMPILER_CLANG
//     #pragma clang diagnostic push
//     #pragma clang diagnostic ignored "-Wmicrosoft-enum-forward-reference"
// #endif
