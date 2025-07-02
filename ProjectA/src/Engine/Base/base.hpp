#pragma once

#include "Engine/config.hpp"

// Determine compiler, OS and architecture.
#include "Engine/Base/context.hpp"

#ifdef BUILD_MODE_DEBUG
	#include "Engine/Base/Debug.hpp"
#endif

// Basic types.
#include "Engine/Base/types.hpp"

static constexpr char* ENGINE_NAME = "ProjectA";

// Binary helpers.
#include "Engine/Base/binary.hpp"

// Casting helpers.
#include "Engine/Base/cast.hpp"

// Enum helpers.
#include "Engine/Base/enum.hpp"

// Function helpers.
#include "Engine/Base/function.hpp"

#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
	#define NOGDICAPMASKS
	#define NOVIRTUALKEYCODES
	#define NOSYSMETRICS
	#define NOMENUS
	#define NOICONS
	#define NOKEYSTATES
	#define NOSYSCOMMANDS
	#define NORASTEROPS
	#define OEMRESOURCE
	#define NOATOM
	#define NOCLIPBOARD
	#define NODRAWTEXT
	#define NOKERNEL
	#define NOMEMMGR
	#define NOMETAFILE
	#define NOMINMAX
	#define NOOPENFILE
	#define NOSCROLL
	#define NOSERVICE
	#define NOSOUND
	#define NOTEXTMETRIC
	#define NOWH
	#define NOWINOFFSETS
	#define NOCOMM
	#define NOKANJI
	#define NOHELP
	#define NOPROFILER
	#define NODEFERWINDOWPOS
	#define NOMCX

	#include <Windows.h>

	// NOTE: ERROR is a LogLevel value.
	#undef ERROR

	#include <windowsx.h>

	// Load Windows 11 theme.
	// https://learn.microsoft.com/en-us/answers/questions/1184393/when-including-comctl32-lib-causes-the-ordinal-345
	#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df'\"")
#endif

#ifdef BUILD_MODE_DEBUG
	#define DEBUG_ASSERT(condition) { \
		if (!(condition)) { \
			ENGINE_ERROR("Debug assertion failed."); \
			BREAKPOINT(); \
		} \
	}

	#define DEBUG_STATIC_ASSERT static_assert
#else
	#define DEBUG_ASSERT
	#define DEBUG_STATIC_ASSERT
#endif

#include "Engine/Extent.hpp"
#include "Engine/Logger/Logger.hpp"

#define ENGINE_TRACE(argumentOrPattern, ...) ::ProjectA::Logger::engineLogger.trace(argumentOrPattern "\n    at []\n    ([]:[])", ##__VA_ARGS__, __FUNCSIG__, __FILE__, __LINE__)
#define ENGINE_TRACE_PREFORMATTED(...) ::ProjectA::Logger::engineLogger.trace(__VA_ARGS__)
#define ENGINE_DEBUG(...) ::ProjectA::Logger::engineLogger.debug(__VA_ARGS__)
#define ENGINE_INFO(...) ::ProjectA::Logger::engineLogger.info(__VA_ARGS__)
#define ENGINE_WARN(...) ::ProjectA::Logger::engineLogger.warn(__VA_ARGS__)
#define ENGINE_ERROR(...) ::ProjectA::Logger::engineLogger.error(__VA_ARGS__)
#define ENGINE_FATAL(...) ::ProjectA::Logger::engineLogger.fatal(__VA_ARGS__)

#define CLIENT_TRACE(...) ::ProjectA::Logger::clientLogger.trace(__VA_ARGS__)
#define CLIENT_DEBUG(...) ::ProjectA::Logger::clientLogger.debug(__VA_ARGS__)
#define CLIENT_INFO(...) ::ProjectA::Logger::clientLogger.info(__VA_ARGS__)
#define CLIENT_WARN(...) ::ProjectA::Logger::clientLogger.warn(__VA_ARGS__)
#define CLIENT_ERROR(...) ::ProjectA::Logger::clientLogger.error(__VA_ARGS__)
#define CLIENT_FATAL(...) ::ProjectA::Logger::clientLogger.fatal(__VA_ARGS__)

#define ENGINE_ASSERT(condition, ...) { \
	if (!(condition)) { \
		ENGINE_ERROR(__VA_ARGS__); \
		BREAKPOINT(); \
	} \
}

#define CLIENT_ASSERT(condition, ...) { \
	if (!(condition)) { \
		CLIENT_ERROR(__VA_ARGS__); \
		BREAKPOINT(); \
	} \
}

#define ENGINE_STATIC_ASSERT static_assert

#include "Engine/Base/UDL.hpp"

#ifdef ENABLE_OPENGL_ADAPTER
	#include <GL/glcorearb.h>

	#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
		#include <gl/GL.h>
	#endif

	#include <GL/glext.h>

	#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
		#include <GL/wglext.h>
	#endif

	#ifndef OPENGL_USE_SPIRV
		// NOTE: <regex> is used for custom entrypoints within a GLSL shader.
		#include <regex>
	#endif

	#include "Backend/OpenGL/procedures.hpp"
#endif

#ifdef ENABLE_VULKAN_ADAPTER
	#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
		#define VK_USE_PLATFORM_WIN32_KHR
	#endif

	#include <vulkan/vulkan.h>

	#ifdef BUILD_MODE_DEBUG
		#include <vulkan/vk_enum_string_helper.h>
	#endif
#endif

#include "Engine/sequence.hpp"

#ifdef BUILD_MODE_PROFILE
	#include "Engine/Profile/Profiler.hpp"
	#include "Engine/Profile/Timer.hpp"

	#define PROFILER(path) ::ProjectA::Profiler profiler = ::ProjectA::Profiler(path)
	#define PROFILE_SCOPE(name) ::ProjectA::Timer timer = ::ProjectA::Timer(name)

	#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
		#define FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__DMC__) && (__DMC__ >= 0x810)
		#define FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__FUNCSIG__) || (_MSC_VER)
		#define FUNC_SIG __FUNCSIG__
	#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
		#define FUNC_SIG __FUNCTION__
	#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
		#define FUNC_SIG __FUNC__
	#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
		#define FUNC_SIG __func__
	#elif defined(__cplusplus) && (__cplusplus >= 201103)
		#define FUNC_SIG __func__
	#else
		#define FUNC_SIG
	#endif
#else
	#define FUNC_SIG
	#define PROFILER(path)
	#define PROFILE_SCOPE(name)
#endif

#include "Engine/Base/string.hpp"