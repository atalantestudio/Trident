#pragma once

#include <Base/Base.hpp>

#define STANDARD ATL_STANDARD
#define STANDARD_OTHER ATL_STANDARD_OTHER
#define STANDARD_CPP11 ATL_STANDARD_CPP11
#define STANDARD_CPP14 ATL_STANDARD_CPP14
#define STANDARD_CPP17 ATL_STANDARD_CPP17
#define STANDARD_CPP20 ATL_STANDARD_CPP20
#define STANDARD_CPP23 ATL_STANDARD_CPP23

#define COMPILER ATL_COMPILER
#define COMPILER_OTHER ATL_COMPILER_OTHER
#define COMPILER_CLANG ATL_COMPILER_CLANG
#define COMPILER_GCC ATL_COMPILER_GCC
#define COMPILER_MSVC ATL_COMPILER_MSVC

#define OPERATING_SYSTEM ATL_OPERATING_SYSTEM
#define OPERATING_SYSTEM_OTHER ATL_OPERATING_SYSTEM_OTHER
#define OPERATING_SYSTEM_LINUX ATL_OPERATING_SYSTEM_LINUX
#define OPERATING_SYSTEM_WINDOWS ATL_OPERATING_SYSTEM_WINDOWS

#define ARCHITECTURE ATL_ARCHITECTURE
#define ARCHITECTURE_OTHER ATL_ARCHITECTURE_OTHER
#define ARCHITECTURE_AMD64 ATL_ARCHITECTURE_AMD64
#define ARCHITECTURE_INTEL_X86 ATL_ARCHITECTURE_INTEL_X86

#define CONSTEXPR_CPP17 ATL_CONSTEXPR_CPP17

#define BIT ATL_BIT

#define ABORT ATL_ABORT
#define ASSERT ATL_ASSERT
#define BREAKPOINT ATL_BREAKPOINT

#if STANDARD < STANDARD_CPP11
	#error "Unsupported C++ version."
#endif

#if COMPILER == COMPILER_CLANG
	#error "Clang is not supported."
#elif COMPILER == COMPILER_GCC
	#error "GCC is not supported."
#elif COMPILER == COMPILER_OTHER
	#error "Unknown compiler."
#endif

#if OPERATING_SYSTEM == OPERATING_SYSTEM_LINUX
	#error "Linux is not supported."
#elif OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS && !defined(_WIN64)
	#error "32-bit Windows is not supported."
#elif OPERATING_SYSTEM == OPERATING_SYSTEM_OTHER
	#error "Unknown operating system."
#endif

#if ARCHITECTURE == ARCHITECTURE_X86
	#error "x86 architecture is not supported."
#elif ARCHITECTURE == ARCHITECTURE_OTHER
	#error "Unknown architecture."
#endif

#if defined(BUILD_MODE_DEBUG)
	#define BUILD_MODE BUILD_MODE_DEBUG
#elif defined(BUILD_MODE_PROFILE)
	#define BUILD_MODE BUILD_MODE_PROFILE
#elif defined(BUILD_MODE_RELEASE)
	#define BUILD_MODE BUILD_MODE_RELEASE
#endif

namespace ProjectA {
	using atl::sint8;
	using atl::sint16;
	using atl::sint32;
	using atl::sint64;

	using atl::uint8;
	using atl::uint16;
	using atl::uint32;
	using atl::uint64;

	using atl::char8;

	using atl::float32;
	using atl::float64;

	using atl::index;

	using atl::range;
	using atl::sequence;
	using atl::view;

	using atl::countBits;
	using atl::countSetBits;
	using atl::find;
	using atl::trim;

	using atl::pointer_cast;

	using atl::max;
	using atl::min;
}