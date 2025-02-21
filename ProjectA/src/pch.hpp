#pragma once

// TODO: Remove
#define _USE_MATH_DEFINES // cmath

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream> // std::ofstream (profile-only)
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread> // std::thread::id (profile-only)
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#ifdef BUILD_MODE_DEBUG
	#include <chrono>
	#include <iomanip>
#endif

#ifdef PLATFORM_WINDOWS
	#include <Windows.h>
#endif

#undef assert
#undef max
#undef min
#undef DELETE
#undef ERROR

#include "ProjectA/base.hpp"

#include "lyah/lyah.hpp"

#include "ProjectA/types.hpp"

// Lyah polyfill: SIMD to scalar vector
#include "ProjectA/vec_sts.hpp"

#ifdef BUILD_MODE_DEBUG
	#include "ProjectA/Logger/Logger.hpp"

	// TODO: Include to string methods in Logger.cpp
	#include "ProjectA/to_string.hpp"
#endif

#ifdef BUILD_MODE_PROFILE
	#include "ProjectA/Profile/Profiler.hpp"
	#include "ProjectA/Profile/Timer.hpp"
#endif