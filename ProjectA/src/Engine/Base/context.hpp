#pragma once

#define COMPILER_CLANG 0
#define COMPILER_GCC 1
#define COMPILER_MSVC 2

// https://sourceforge.net/p/predef/wiki/Compilers#clang
#ifdef __clang__
	#define COMPILER COMPILER_CLANG

	// TODO: Add Clang support.
	#error "Clang is not supported."
#endif

// https://sourceforge.net/p/predef/wiki/Compilers#gcc-cc
#ifdef __GNUC__
	#define COMPILER COMPILER_GCC

	// TODO: Add GCC support.
	#error "GCC is not supported."
#endif

// https://sourceforge.net/p/predef/wiki/Compilers#microsoft-visual-c
#ifdef _MSC_VER
	#define COMPILER COMPILER_MSVC
#endif

#ifndef COMPILER
	#error "Unsupported compiler."
#endif

#define OPERATING_SYSTEM_LINUX 0
#define OPERATING_SYSTEM_WINDOWS 1

// https://sourceforge.net/p/predef/wiki/OperatingSystems#gnulinux
#ifdef __gnu_linux__
	#define OPERATING_SYSTEM OPERATING_SYSTEM_LINUX

	// TODO: Add Linux support.
	#error "Linux is not supported."
#endif

// https://sourceforge.net/p/predef/wiki/OperatingSystems#windows
#ifdef _WIN32
	#define OPERATING_SYSTEM OPERATING_SYSTEM_WINDOWS

	#ifndef _WIN64
		#error "32-bit Windows is not supported."
	#endif
#endif

#ifndef OPERATING_SYSTEM
	#error "Unsupported operating system."
#endif

#define ARCHITECTURE_X86 0
#define ARCHITECTURE_X86_64 1

// https://sourceforge.net/p/predef/wiki/Architectures/amd64
#ifdef __amd64__
	#define ARCHITECTURE ARCHITECTURE_X86_64
#endif

// https://sourceforge.net/p/predef/wiki/Architectures/amd64
#ifdef _M_X64
	#define ARCHITECTURE ARCHITECTURE_X86_64
#endif

// https://sourceforge.net/p/predef/wiki/Architectures/intel-x86
#ifdef __i386__
	#define ARCHITECTURE ARCHITECTURE_X86

	#error "x86 architecture is not supported."
#endif

// https://sourceforge.net/p/predef/wiki/Architectures/intel-x86
#ifdef _M_IX86
	#define ARCHITECTURE ARCHITECTURE_X86

	#error "x86 architecture is not supported."
#endif

#ifndef ARCHITECTURE
	#error "Unsupported architecture."
#endif