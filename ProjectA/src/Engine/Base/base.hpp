#pragma once

#if defined(BUILD_MODE_DEBUG)
	#define BUILD_MODE BUILD_MODE_DEBUG
#elif defined(BUILD_MODE_PROFILE)
	#define BUILD_MODE BUILD_MODE_PROFILE
#elif defined(BUILD_MODE_RELEASE)
	#define BUILD_MODE BUILD_MODE_RELEASE
#endif

#define USER_NAMESPACE ProjectA

#include <Base/Base.hpp>

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

#include "Engine/config.hpp"

#include "Engine/Base/Macros.hpp"
#include "Engine/Base/types.hpp"
#include "Engine/Base/binary.hpp"
#include "Engine/Base/cast.hpp"
#include "Engine/Base/enum.hpp"
#include "Engine/Base/Endianness.hpp"
#include "Engine/Base/UDL.hpp"

#include "Engine/Base/Platform/Platform.hpp"

#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
	#include "Engine/Base/Windows.hpp"
#endif

#ifdef ENABLE_OPENGL_ADAPTER
	#include "Backend/OpenGL/base.hpp"
#endif

#ifdef ENABLE_VULKAN_ADAPTER
	#include "Backend/Vulkan/base.hpp"
#endif

#include "Engine/Base/Random.hpp"
#include "Engine/Base/VersionNumber.hpp"