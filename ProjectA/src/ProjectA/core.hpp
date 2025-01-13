#pragma once

#include <memory>

#ifdef _WIN32
	#ifdef _WIN64
		// TODO: Rename by PLATFORM_WINDOWS or similar
		#define WINDOWS
	#else
		#error "Windows x86 builds are not supported."
	#endif
#elif defined(__linux__)
	// TODO: Rename by PLATFORM_LINUX or similar
	#define LINUX

	#error "Operating system is not supported."
#else
	#error "Unknown operating system."
#endif

#ifdef BUILD_MODE_DEBUG
	#define ENABLE_ASSERTS
#endif

#ifdef BUILD_MODE_PROFILE
	#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
		#define FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__DMC__) && (__DMC__ >= 0x810)
		#define FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__FUNCSIG__)
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

#ifdef ENABLE_ASSERTS
	#define ENGINE_ASSERT(condition, ...) { \
		if (!condition) { \
			ENGINE_ERROR("Assertion failed: []", __VA_ARGS__); \
			__debugbreak(); \
		} \
	}

	#define CLIENT_ASSERT(condition, ...) { \
		if (!condition) { \
			CLIENT_ERROR("Assertion failed: []", __VA_ARGS__); \
			__debugbreak(); \
		} \
	}
#else
	#define ENGINE_ASSERT(condition, ...)
	#define CLIENT_ASSERT(condition, ...)
#endif

#define BIND(function) std::bind(&function, this, std::placeholders::_1)

#define AS_FLAG(T) \
	inline bool operator &(T a, T b) { \
		return static_cast<bool>(static_cast<int32>(a) & static_cast<int32>(b)); \
	} \
	\
	inline T operator |(T a, T b) { \
		return static_cast<T>(static_cast<int32>(a) | static_cast<int32>(b)); \
	}

#define AS_PRINTABLE(T) \
	inline std::ostream& operator <<(std::ostream& stream, T a) { \
		return stream << static_cast<int32>(a); \
	}