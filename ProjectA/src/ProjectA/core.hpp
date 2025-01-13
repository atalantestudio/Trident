#pragma once

#include <memory>

#ifdef _WIN32
	#ifdef _WIN64
		#define WINDOWS
	#else
		#error "x86 builds are not supported."
	#endif
#elif defined(__linux__)
	#define LINUX

	#error "Operating system not supported."
#else
	#error "Unknown operating system."
#endif

#ifdef WINDOWS
	#ifdef DYNAMIC_LINK
		#ifdef BUILD_DLL
			#define API __declspec(dllexport)
		#else
			#define API __declspec(dllimport)
		#endif
	#else
		#define API
	#endif
#else
	#error "Operating system not supported."
#endif

#ifdef BUILD_MODE_DEBUG
	#define ASSERTS
#endif

#ifdef ASSERTS
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