#pragma once

#include <array>
#include <fstream>
#include <functional>
#include <sstream>

#ifdef BUILD_MODE_DEBUG
	#include <chrono>
	#include <iomanip>
	#include <iostream>
#endif

#ifdef BUILD_MODE_PROFILE
	#include <algorithm>
	#include <thread>
#endif

#ifdef PLATFORM_WINDOWS
	#include <Windows.h>

	#undef assert
	#undef max
	#undef min
	#undef DELETE
	#undef ERROR
#endif

#include "Engine/base.hpp"

#include "lyah/lyah.hpp"

#include "Engine/types.hpp"

// Lyah polyfill: SIMD to scalar vector
#include "Engine/vec_sts.hpp"

#ifdef BUILD_MODE_DEBUG
	#include "Engine/Logger/Logger.hpp"

	// TODO: Include ToString methods in Logger.cpp
	#include "Engine/to_string.hpp"
#endif

#ifdef BUILD_MODE_PROFILE
	#include "Engine/Profile/Profiler.hpp"
	#include "Engine/Profile/Timer.hpp"
#endif