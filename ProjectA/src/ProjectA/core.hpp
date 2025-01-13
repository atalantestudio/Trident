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