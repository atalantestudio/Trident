#pragma once

#include <array>
#include <bitset>
#include <filesystem>
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

#include "Engine/base.hpp"

#ifdef PLATFORM_WINDOWS
	#include <Windows.h>

	#undef assert
	#undef DELETE
	#undef ERROR
	#undef far
	#undef max
	#undef min
	#undef near
#endif

#include "lyah/lyah.hpp"

#include "Engine/types.hpp"

// Lyah polyfill: SIMD to scalar vector.
#include "Engine/vec_sts.hpp"

#ifdef BUILD_MODE_DEBUG
	#include "Engine/Logger/Logger.hpp"

	// TODO: Include this inside Logger.
	#include "Engine/to_string.hpp"
#endif

#ifdef BUILD_MODE_PROFILE
	#include "Engine/Profile/Profiler.hpp"
	#include "Engine/Profile/Timer.hpp"
#endif