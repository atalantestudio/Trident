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

#include "ProjectA/base.hpp"

#include "lyah/lyah.hpp"

#include "ProjectA/types.hpp"

// Lyah polyfill: SIMD to scalar vector
#include "ProjectA/vec_sts.hpp"

#ifdef BUILD_MODE_DEBUG
	#include "ProjectA/Logger/Logger.hpp"

	// TODO: Include ToString methods in Logger.cpp
	#include "ProjectA/to_string.hpp"
#endif

#ifdef BUILD_MODE_PROFILE
	#include "ProjectA/Profile/Profiler.hpp"
	#include "ProjectA/Profile/Timer.hpp"
#endif