#pragma once

#define ENGINE_NAME "ProjectA"

#ifdef _WIN32
	#ifdef _WIN64
		#define PLATFORM_WINDOWS
	#else
		#error "Unhandled platform."
	#endif
#elif defined(__linux__)
	#define PLATFORM_LINUX

	#include <signal.h>

	#error "Unhandled platform."
#else
	#error "Unhandled platform."
#endif

#ifdef BUILD_MODE_DEBUG
	#define ENGINE_TRACE(...) ProjectA::Logger::engineLogger->trace(__VA_ARGS__)
	#define ENGINE_DEBUG(...) ProjectA::Logger::engineLogger->debug(__VA_ARGS__)
	#define ENGINE_INFO(...) ProjectA::Logger::engineLogger->info(__VA_ARGS__)
	#define ENGINE_WARN(...) ProjectA::Logger::engineLogger->warn(__VA_ARGS__)
	#define ENGINE_ERROR(...) ProjectA::Logger::engineLogger->error(__VA_ARGS__)
	#define ENGINE_FATAL(...) ProjectA::Logger::engineLogger->fatal(__VA_ARGS__)

	#define CLIENT_TRACE(...) ProjectA::Logger::clientLogger->trace(__VA_ARGS__)
	#define CLIENT_DEBUG(...) ProjectA::Logger::clientLogger->debug(__VA_ARGS__)
	#define CLIENT_INFO(...) ProjectA::Logger::clientLogger->info(__VA_ARGS__)
	#define CLIENT_WARN(...) ProjectA::Logger::clientLogger->warn(__VA_ARGS__)
	#define CLIENT_ERROR(...) ProjectA::Logger::clientLogger->error(__VA_ARGS__)
	#define CLIENT_FATAL(...) ProjectA::Logger::clientLogger->fatal(__VA_ARGS__)

	#define DEBUG_ENABLE_MESSENGER
	#define DEBUG_ENABLE_VALIDATION_LAYERS
#else
	#define ENGINE_TRACE(...)
	#define ENGINE_DEBUG(...)
	#define ENGINE_INFO(...)
	#define ENGINE_WARN(...)
	#define ENGINE_ERROR(...)
	#define ENGINE_FATAL(...)

	#define CLIENT_TRACE(...)
	#define CLIENT_DEBUG(...)
	#define CLIENT_INFO(...)
	#define CLIENT_WARN(...)
	#define CLIENT_ERROR(...)
	#define CLIENT_FATAL(...)
#endif

#if defined(PLATFORM_WINDOWS)
	#define BREAKPOINT() __debugbreak()
#elif defined(__clang__)
	#define BREAKPOINT()  __builtin_debugtrap()
#elif defined(PLATFORM_LINUX)
	#define BREAKPOINT() raise(SIGTRAP)
#else
	#error "Unhandled platform."
#endif

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

#ifdef BUILD_MODE_PROFILE
	#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
		#define FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__DMC__) && (__DMC__ >= 0x810)
		#define FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(FUNC_SIG)
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

	#define PROFILER(path) ::ProjectA::Profiler profiler(path)

	#define PROFILE_SCOPE(name) ::ProjectA::Timer timer(name)
#else
	#define FUNC_SIG
	#define PROFILER(path)
	#define PROFILE_SCOPE(name)
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