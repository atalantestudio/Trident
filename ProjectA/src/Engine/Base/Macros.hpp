#pragma once

#define ENGINE_STATIC_ASSERT static_assert

// TODO
// #ifdef BUILD_MODE_PROFILE
#if 0
	#include "Engine/Profile/Profiler.hpp"
	#include "Engine/Profile/ProfileTimer.hpp"

	#define PROFILER(path) ::ProjectA::Profiler __profiler = ::ProjectA::Profiler(path)
	#define PROFILE_SCOPE(name) ::ProjectA::ProfileTimer __profileTimer = ::ProjectA::ProfileTimer(name)

	#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
		#define FUNCTION_SIGNATURE __PRETTY_FUNCTION__
	#elif defined(__DMC__) && (__DMC__ >= 0x810)
		#define FUNCTION_SIGNATURE __PRETTY_FUNCTION__
	#elif defined(__FUNCSIG__) || (_MSC_VER)
		#define FUNCTION_SIGNATURE __FUNCSIG__
	#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
		#define FUNCTION_SIGNATURE __FUNCTION__
	#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
		#define FUNCTION_SIGNATURE __FUNC__
	#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
		#define FUNCTION_SIGNATURE __func__
	#elif defined(__cplusplus) && (__cplusplus >= 201103)
		#define FUNCTION_SIGNATURE __func__
	#else
		#define FUNCTION_SIGNATURE
	#endif
#else
	#define FUNCTION_SIGNATURE
	#define PROFILER(...)
	#define PROFILE_SCOPE(...)
#endif

// TODO: Check min C++ version for this code.
#define BIND(function) [this](auto&&... arguments) { \
	return this->function(std::forward<decltype(arguments)>(arguments)...); \
}