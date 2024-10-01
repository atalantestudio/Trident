#pragma once

#include <memory>

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
	#error Operating system not supported.
#endif

#ifdef DEBUG
	#define ASSERTS
#endif

#ifdef ASSERTS
	#define CORE_ASSERT(condition, ...) { \
		if (!condition) { \
			LOG_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); \
			__debugbreak(); \
		} \
	}

	#define CLIENT_ASSERT(condition, ...) { \
		if (!condition) { \
			LOG_CLIENT_ERROR("Assertion failed: {0}", __VA_ARGS__); \
			__debugbreak(); \
		} \
	}
#else
	#define CORE_ASSERT(condition, ...)
	#define CLIENT_ASSERT(condition, ...)
#endif

#define BIT_SHIFT_ID(id) (1 << id)

#define BIND(function) std::bind(&function, this, std::placeholders::_1)

namespace ProjectA {
	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T>
	using Scope = std::unique_ptr<T>;
}