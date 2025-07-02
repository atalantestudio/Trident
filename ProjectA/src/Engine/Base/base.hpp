#pragma once

#include "Engine/config.hpp"

// Determine compiler, OS and architecture.
#include "Engine/Base/context.hpp"

// Basic types.
#include "Engine/Base/types.hpp"

// Binary helpers.
#include "Engine/Base/binary.hpp"

// Casting helpers.
#include "Engine/Base/cast.hpp"

// Enum helpers.
#include "Engine/Base/enum.hpp"

// Function helpers.
#include "Engine/Base/function.hpp"

#define ENGINE_NAME "ProjectA"
#define ENGINE_NAME_L L"ProjectA"

#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
	#define VK_USE_PLATFORM_WIN32_KHR

	#include <Windows.h>
	#include <windowsx.h>

	// TODO: Refactor in context.hpp.
	// https://learn.microsoft.com/en-us/answers/questions/1184393/when-including-comctl32-lib-causes-the-ordinal-345
	#if defined _M_IX86
		#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#elif defined _M_IA64
		#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#elif defined _M_X64
		#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#else
		#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#endif

	#undef assert
	#undef DELETE
	#undef ERROR // LogLevel::ERROR
	#undef far // lyah
	#undef max // lyah::max
	#undef min // lyah::min
	#undef near // lyah
#endif

#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
	#define BREAKPOINT() __debugbreak()
#elif defined(__clang__)
	#define BREAKPOINT() __builtin_debugtrap()
#elif defined(PLATFORM_LINUX)
	#define BREAKPOINT() raise(SIGTRAP)
#else
	#error "Unsupported platform."
#endif

#ifdef BUILD_MODE_DEBUG
	#define DEBUG_ENABLE_MESSENGER
	#define DEBUG_ENABLE_VALIDATION_LAYERS

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

namespace ProjectA {
	#define ENGINE_TRACE(argumentOrPattern, ...) Logger::engineLogger.trace(argumentOrPattern "\n    at []\n    ([]:[])", ##__VA_ARGS__, __FUNCSIG__, __FILE__, __LINE__)
	#define ENGINE_TRACE_PREFORMATTED(...) Logger::engineLogger.trace(__VA_ARGS__)
	#define ENGINE_DEBUG(...) Logger::engineLogger.debug(__VA_ARGS__)
	#define ENGINE_INFO(...) Logger::engineLogger.info(__VA_ARGS__)
	#define ENGINE_WARN(...) Logger::engineLogger.warn(__VA_ARGS__)
	#define ENGINE_ERROR(...) Logger::engineLogger.error(__VA_ARGS__)
	#define ENGINE_FATAL(...) Logger::engineLogger.fatal(__VA_ARGS__)

	#define CLIENT_TRACE(...) Logger::clientLogger.trace(__VA_ARGS__)
	#define CLIENT_DEBUG(...) Logger::clientLogger.debug(__VA_ARGS__)
	#define CLIENT_INFO(...) Logger::clientLogger.info(__VA_ARGS__)
	#define CLIENT_WARN(...) Logger::clientLogger.warn(__VA_ARGS__)
	#define CLIENT_ERROR(...) Logger::clientLogger.error(__VA_ARGS__)
	#define CLIENT_FATAL(...) Logger::clientLogger.fatal(__VA_ARGS__)
}

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
	#include <vulkan/vulkan.h>

	#ifdef BUILD_MODE_DEBUG
		#include <vulkan/vk_enum_string_helper.h>
	#endif
#endif

#include "Engine/Module/lyah.hpp"
#include "Engine/Extent.hpp"

#include "Engine/sequence.hpp"
#include "Engine/Logger/Logger.hpp"

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