#pragma once

// OS Detection

#if defined(_WIN32) || defined(_WIN64)
#define COVSCRIPT_PLATFORM_WIN32
#ifdef _UCRT
#define COVSCRIPT_PLATFORM_NAME "windows-ucrt"
#else
#define COVSCRIPT_PLATFORM_NAME "windows-msvcrt"
#endif
#define COVSCRIPT_PLATFORM_HOME "C:\\Program Files (x86)\\CovScript"
#elif defined(__ANDROID__)
#define COVSCRIPT_PLATFORM_ANDROID
#define COVSCRIPT_PLATFORM_LINUX
#define COVSCRIPT_PLATFORM_UNIX
#define COVSCRIPT_PLATFORM_NAME "android"
#elif defined(__linux__) || defined(__gnu_linux__)
#define COVSCRIPT_PLATFORM_LINUX
#define COVSCRIPT_PLATFORM_UNIX
#define COVSCRIPT_PLATFORM_NAME "linux"
#ifndef COVSCRIPT_PLATFORM_HOME
#define COVSCRIPT_PLATFORM_HOME "/usr/share/covscript"
#endif
#elif defined(__APPLE__) && defined(__MACH__)
#define COVSCRIPT_PLATFORM_DARWIN
#define COVSCRIPT_PLATFORM_UNIX
#define COVSCRIPT_PLATFORM_NAME "macos"
#ifndef COVSCRIPT_PLATFORM_HOME
#define COVSCRIPT_PLATFORM_HOME "/Applications/CovScript.app/Contents/MacOS/covscript"
#endif
#elif defined(__FreeBSD__)
#define COVSCRIPT_PLATFORM_FREEBSD
#define COVSCRIPT_PLATFORM_UNIX
#define COVSCRIPT_PLATFORM_NAME "freebsd"
#ifndef COVSCRIPT_PLATFORM_HOME
#define COVSCRIPT_PLATFORM_HOME "/usr/local/share/covscript"
#endif
#else
#define COVSCRIPT_PLATFORM_UNKNOWN
#warning Platform detection failed. Using unix mode as fallback.
#define COVSCRIPT_PLATFORM_UNIX
#ifndef COVSCRIPT_PLATFORM_NAME
#error COVSCRIPT_PLATFORM_NAME not defined. Can not detect automatically.
#endif
#ifndef COVSCRIPT_PLATFORM_HOME
#error COVSCRIPT_PLATFORM_HOME not defined. Can not detect automatically.
#endif
#endif

// Compiler Detection

#if defined(__clang__)
#define COVSCRIPT_COMPILER_CLANG
#define COVSCRIPT_COMPILER_SIMPLE_NAME "clang"
#elif defined(__GNUC__)
#define COVSCRIPT_COMPILER_GNUC
#define COVSCRIPT_COMPILER_SIMPLE_NAME "gcc"
#elif defined(_MSC_VER)
#define COVSCRIPT_COMPILER_MSVC
#define COVSCRIPT_COMPILER_SIMPLE_NAME "msvc"
#else
#define COVSCRIPT_COMPILER_UNKNOWN
#define COVSCRIPT_COMPILER_SIMPLE_NAME "unknown"
#endif

// MinGW suffix

#if defined(__MINGW64__)
#undef COVSCRIPT_COMPILER_NAME
#define COVSCRIPT_COMPILER_NAME COVSCRIPT_COMPILER_SIMPLE_NAME "-mingw-w64"
#define COVSCRIPT_COMPILER_MINGW
#define COVSCRIPT_COMPILER_MINGW64
#elif defined(__MINGW32__)
#undef COVSCRIPT_COMPILER_NAME
#define COVSCRIPT_COMPILER_NAME COVSCRIPT_COMPILER_SIMPLE_NAME "-mingw"
#define COVSCRIPT_COMPILER_MINGW
#define COVSCRIPT_COMPILER_MINGW32
#endif

#ifndef COVSCRIPT_COMPILER_NAME
#define COVSCRIPT_COMPILER_NAME COVSCRIPT_COMPILER_SIMPLE_NAME
#endif

// Architecture Detection

#if defined(__x86_64__) || defined(_M_X64)
#define COVSCRIPT_ARCH_AMD64
#define COVSCRIPT_ARCH_NAME "amd64"
#elif defined(__i386__) || defined(_M_IX86)
#define COVSCRIPT_ARCH_I386
#define COVSCRIPT_ARCH_NAME "i386"
#elif defined(__aarch64__) || defined(_M_ARM64)
#define COVSCRIPT_ARCH_ARM64
#ifdef COVSCRIPT_PLATFORM_LINUX
#define COVSCRIPT_ARCH_NAME "aarch64"
#else
#define COVSCRIPT_ARCH_NAME "arm64"
#endif
#elif defined(__arm__) || defined(_M_ARM)
#define COVSCRIPT_ARCH_ARM32
#define COVSCRIPT_ARCH_NAME "arm"
#elif defined(__riscv)
#if defined(__riscv_xlen) && (__riscv_xlen == 64)
#define COVSCRIPT_ARCH_RISCV64
#define COVSCRIPT_ARCH_NAME "riscv64"
#else
#define COVSCRIPT_ARCH_RISCV32
#define COVSCRIPT_ARCH_NAME "riscv32"
#endif
#elif defined(__loongarch__)
#if defined(__loongarch64)
#define COVSCRIPT_ARCH_LOONGARCH64
#define COVSCRIPT_ARCH_NAME "loongarch64"
#else
#define COVSCRIPT_ARCH_LOONGARCH32
#define COVSCRIPT_ARCH_NAME "loongarch32"
#endif
#elif defined(__mips__)
#if defined(__mips64) || defined(_LP64)
#define COVSCRIPT_ARCH_MIPS64
#define COVSCRIPT_ARCH_NAME "mips64"
#else
#define COVSCRIPT_ARCH_MIPS32
#define COVSCRIPT_ARCH_NAME "mips32"
#endif
#else
#define COVSCRIPT_ARCH_UNKNOWN
#ifndef COVSCRIPT_ARCH_NAME
#error COVSCRIPT_ARCH_NAME not defined. Can not detect automatically.
#endif
#endif

namespace cs {
	// Path seperator and delimiter
#ifdef COVSCRIPT_PLATFORM_WIN32
	constexpr char path_separator = '\\';
	constexpr char path_delimiter = ';';
#else
	constexpr char path_separator = '/';
	constexpr char path_delimiter = ':';
#endif
}