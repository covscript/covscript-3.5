#pragma once

// OS Detection

#if defined(_WIN32) || defined(_WIN64)
#define CSVM_PLATFORM_WIN32
#ifdef _UCRT
#define CSVM_PLATFORM_NAME "windows-ucrt"
#else
#define CSVM_PLATFORM_NAME "windows-msvcrt"
#endif
#define CSVM_PLATFORM_HOME "C:\\Program Files (x86)\\CovScript"
#elif defined(__ANDROID__)
#define CSVM_PLATFORM_ANDROID
#define CSVM_PLATFORM_LINUX
#define CSVM_PLATFORM_UNIX
#define CSVM_PLATFORM_NAME "android"
#elif defined(__linux__) || defined(__gnu_linux__)
#define CSVM_PLATFORM_LINUX
#define CSVM_PLATFORM_UNIX
#define CSVM_PLATFORM_NAME "linux"
#ifndef CSVM_PLATFORM_HOME
#define CSVM_PLATFORM_HOME "/usr/share/covscript"
#endif
#elif defined(__APPLE__) && defined(__MACH__)
#define CSVM_PLATFORM_DARWIN
#define CSVM_PLATFORM_UNIX
#define CSVM_PLATFORM_NAME "macos"
#ifndef CSVM_PLATFORM_HOME
#define CSVM_PLATFORM_HOME "/Applications/CovScript.app/Contents/MacOS/covscript"
#endif
#elif defined(__FreeBSD__)
#define CSVM_PLATFORM_FREEBSD
#define CSVM_PLATFORM_UNIX
#define CSVM_PLATFORM_NAME "freebsd"
#ifndef CSVM_PLATFORM_HOME
#define CSVM_PLATFORM_HOME "/usr/local/share/covscript"
#endif
#else
#define CSVM_PLATFORM_UNKNOWN
#warning Platform detection failed. Using unix mode as fallback.
#define CSVM_PLATFORM_UNIX
#ifndef CSVM_PLATFORM_NAME
#error CSVM_PLATFORM_NAME not defined. Can not detect automatically.
#endif
#ifndef CSVM_PLATFORM_HOME
#error CSVM_PLATFORM_HOME not defined. Can not detect automatically.
#endif
#endif

// Compiler Detection

#if defined(__clang__)
#define CSVM_COMPILER_CLANG
#define CSVM_COMPILER_SIMPLE_NAME "clang"
#elif defined(__GNUC__)
#define CSVM_COMPILER_GNUC
#define CSVM_COMPILER_SIMPLE_NAME "gcc"
#elif defined(_MSC_VER)
#define CSVM_COMPILER_MSVC
#define CSVM_COMPILER_SIMPLE_NAME "msvc"
#else
#define CSVM_COMPILER_UNKNOWN
#define CSVM_COMPILER_SIMPLE_NAME "unknown"
#endif

// MinGW suffix

#if defined(__MINGW64__)
#undef CSVM_COMPILER_NAME
#define CSVM_COMPILER_NAME CSVM_COMPILER_SIMPLE_NAME "-mingw-w64"
#define CSVM_COMPILER_MINGW
#define CSVM_COMPILER_MINGW64
#elif defined(__MINGW32__)
#undef CSVM_COMPILER_NAME
#define CSVM_COMPILER_NAME CSVM_COMPILER_SIMPLE_NAME "-mingw"
#define CSVM_COMPILER_MINGW
#define CSVM_COMPILER_MINGW32
#endif

#ifndef CSVM_COMPILER_NAME
#define CSVM_COMPILER_NAME CSVM_COMPILER_SIMPLE_NAME
#endif

// Architecture Detection

#if defined(__x86_64__) || defined(_M_X64)
#define CSVM_ARCH_AMD64
#define CSVM_ARCH_NAME "amd64"
#elif defined(__i386__) || defined(_M_IX86)
#define CSVM_ARCH_I386
#define CSVM_ARCH_NAME "i386"
#elif defined(__aarch64__) || defined(_M_ARM64)
#define CSVM_ARCH_ARM64
#ifdef CSVM_PLATFORM_LINUX
#define CSVM_ARCH_NAME "aarch64"
#else
#define CSVM_ARCH_NAME "arm64"
#endif
#elif defined(__arm__) || defined(_M_ARM)
#define CSVM_ARCH_ARM32
#define CSVM_ARCH_NAME "arm"
#elif defined(__riscv)
#if defined(__riscv_xlen) && (__riscv_xlen == 64)
#define CSVM_ARCH_RISCV64
#define CSVM_ARCH_NAME "riscv64"
#else
#define CSVM_ARCH_RISCV32
#define CSVM_ARCH_NAME "riscv32"
#endif
#elif defined(__loongarch__)
#if defined(__loongarch64)
#define CSVM_ARCH_LOONGARCH64
#define CSVM_ARCH_NAME "loongarch64"
#else
#define CSVM_ARCH_LOONGARCH32
#define CSVM_ARCH_NAME "loongarch32"
#endif
#elif defined(__mips__)
#if defined(__mips64) || defined(_LP64)
#define CSVM_ARCH_MIPS64
#define CSVM_ARCH_NAME "mips64"
#else
#define CSVM_ARCH_MIPS32
#define CSVM_ARCH_NAME "mips32"
#endif
#else
#define CSVM_ARCH_UNKNOWN
#ifndef CSVM_ARCH_NAME
#error CSVM_ARCH_NAME not defined. Can not detect automatically.
#endif
#endif

namespace csvm
{
	// Path seperator and delimiter
#ifdef CSVM_PLATFORM_WIN32
	constexpr char path_separator = '\\';
	constexpr char path_delimiter = ';';
#else
	constexpr char path_separator = '/';
	constexpr char path_delimiter = ':';
#endif
} // namespace csvm

#ifdef __cpp_lib_hardware_interference_size
#define CSVM_CACHELINE_SIZE (std::hardware_constructive_interference_size)
#else
#define CSVM_CACHELINE_SIZE 64
#endif
