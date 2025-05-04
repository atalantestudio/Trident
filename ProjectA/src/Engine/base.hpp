#pragma once

#define ENGINE_NAME "ProjectA"
#define ENGINE_NAME_L L"ProjectA"

// Enable/disable OpenGL graphics adapter.
#define ENABLE_OPENGL

// Enable/disable Vulkan graphics adapter.
#define ENABLE_VULKAN

#ifdef _WIN32
	#ifdef _WIN64
		#define PLATFORM_WINDOWS

		#include <Windows.h>
		#include <windowsx.h>

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
	#else
		#error "Unsupported Windows platform."
	#endif
#elif defined(__linux__)
	#define PLATFORM_LINUX

	#error "Unsupported Linux platform."
#else
	#error "Unsupported platform."
#endif

#ifdef BUILD_MODE_DEBUG
	#define DEBUG_ENABLE_MESSENGER
	#define DEBUG_ENABLE_VALIDATION_LAYERS
#endif

#if defined(PLATFORM_WINDOWS)
	#define BREAKPOINT() __debugbreak()
#elif defined(__clang__)
	#define BREAKPOINT() __builtin_debugtrap()
#elif defined(PLATFORM_LINUX)
	#define BREAKPOINT() raise(SIGTRAP)
#else
	#error "Unsupported platform."
#endif

#define ENGINE_TRACE(...) ProjectA::Logger::engineLogger.trace(__VA_ARGS__)
#define ENGINE_DEBUG(...) ProjectA::Logger::engineLogger.debug(__VA_ARGS__)
#define ENGINE_INFO(...) ProjectA::Logger::engineLogger.info(__VA_ARGS__)
#define ENGINE_WARN(...) ProjectA::Logger::engineLogger.warn(__VA_ARGS__)
#define ENGINE_ERROR(...) ProjectA::Logger::engineLogger.error(__VA_ARGS__)
#define ENGINE_FATAL(...) ProjectA::Logger::engineLogger.fatal(__VA_ARGS__)

#define CLIENT_TRACE(...) ProjectA::Logger::clientLogger.trace(__VA_ARGS__)
#define CLIENT_DEBUG(...) ProjectA::Logger::clientLogger.debug(__VA_ARGS__)
#define CLIENT_INFO(...) ProjectA::Logger::clientLogger.info(__VA_ARGS__)
#define CLIENT_WARN(...) ProjectA::Logger::clientLogger.warn(__VA_ARGS__)
#define CLIENT_ERROR(...) ProjectA::Logger::clientLogger.error(__VA_ARGS__)
#define CLIENT_FATAL(...) ProjectA::Logger::clientLogger.fatal(__VA_ARGS__)

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

#ifdef BUILD_MODE_DEBUG
	#define DEBUG_ASSERT(condition, ...) { \
	if (!(condition)) { \
		ENGINE_ERROR("Debug assertion failed."); \
		BREAKPOINT(); \
	} \
}
#else
	#define DEBUG_ASSERT
#endif

#ifdef BUILD_MODE_PROFILE
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

#define ENGINE_STATIC_ASSERT static_assert

#ifdef ENABLE_OPENGL
	#ifdef PLATFORM_WINDOWS
		#include <gl/GL.h>
	#endif

	#include <GL/glcorearb.h>
	#include <GL/glext.h>

	#ifdef PLATFORM_WINDOWS
		#include <GL/wglext.h>
	#endif

	#include "Backend/OpenGL/procedures.hpp"
#endif

#ifdef ENABLE_VULKAN
	#ifdef PLATFORM_WINDOWS
		#define VK_USE_PLATFORM_WIN32_KHR
	#endif

	#include <vulkan/vulkan.h>
#endif

#define BIT(index) 1 << index

#define STRING_TO_BIG_ENDIAN_UINT32(type) ( \
	(static_cast<uint32>(type[0]) << 24) | \
	(static_cast<uint32>(type[1]) << 16) | \
	(static_cast<uint32>(type[2]) << 8) | \
	(static_cast<uint32>(type[3]) << 0) \
)

#define STRING_TO_LITTLE_ENDIAN_UINT32(type) ( \
	(static_cast<uint32>(type[0]) << 0) | \
	(static_cast<uint32>(type[1]) << 8) | \
	(static_cast<uint32>(type[2]) << 16) | \
	(static_cast<uint32>(type[3]) << 24) \
)

// TODO: Remove usage of auto?
#define BIND(function) \
	[this](auto&&... arguments) -> decltype(auto) { \
		return this->function(std::forward<decltype(arguments)>(arguments)...); \
	}

#define AS_FLAG(T) \
	inline bool operator &(T a, T b) { \
		return static_cast<bool>(static_cast<sint32>(a) & static_cast<sint32>(b)); \
	} \
	\
	inline T operator |(T a, T b) { \
		return static_cast<T>(static_cast<sint32>(a) | static_cast<sint32>(b)); \
	} \
	\
	inline bool operator &(std::underlying_type<T>::type a, T b) { \
		return a & static_cast<std::underlying_type<T>::type>(b); \
	}

#define AS_PRINTABLE(T) \
	inline std::ostream& operator <<(std::ostream& stream, T a) { \
		return stream << static_cast<sint32>(a); \
	}