#pragma once

#define COMPILER_UNSUPPORTED 0
#define COMPILER_CLANG 1
#define COMPILER_GCC 2
#define COMPILER_MSVC 3

#ifdef __clang__
	#define COMPILER COMPILER_CLANG
#endif

#ifdef __GNUC__
	#define COMPILER COMPILER_GCC
#endif

#ifdef _MSC_VER
	#define COMPILER COMPILER_MSVC
#endif

#ifndef COMPILER
	#define COMPILER COMPILER_UNSUPPORTED
#endif

#define OPERATING_SYSTEM_UNSUPPORTED 0
#define OPERATING_SYSTEM_LINUX 1
#define OPERATING_SYSTEM_WINDOWS 2

#ifdef __gnu_linux__
	#define OPERATING_SYSTEM OPERATING_SYSTEM_LINUX

	// TODO: Add Linux support.
	#error "Linux is not supported."
#endif

#ifdef _WIN32
	#define OPERATING_SYSTEM OPERATING_SYSTEM_WINDOWS

	#ifndef _WIN64
		#error "32-bit Windows is not supported."
	#endif
#endif

#ifndef OPERATING_SYSTEM
	#define OPERATING_SYSTEM OPERATING_SYSTEM_UNSUPPORTED
#endif

#define ARCHITECTURE_UNSUPPORTED 0
#define ARCHITECTURE_X86 1
#define ARCHITECTURE_X86_64 2

#ifdef __amd64__
	#define ARCHITECTURE ARCHITECTURE_X86_64
#endif

#ifdef _M_X64
	#define ARCHITECTURE ARCHITECTURE_X86_64
#endif

#ifdef __i386__
	#define ARCHITECTURE ARCHITECTURE_X86

	#error "x86 architecture is not supported."
#endif

#ifdef _M_IX86
	#define ARCHITECTURE ARCHITECTURE_X86

	#error "x86 architecture is not supported."
#endif

#ifndef ARCHITECTURE
	#define ARCHITECTURE ARCHITECTURE_UNSUPPORTED
#endif