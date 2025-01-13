#pragma once

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

#ifdef WINDOWS
	#include <Windows.h>
#endif

#undef assert
#undef DELETE
#undef ERROR

#include "ProjectA/types.hpp"

#ifdef BUILD_MODE_DEBUG
	#include "ProjectA/Logger/Logger.hpp"
#endif

#ifdef BUILD_MODE_PROFILE
	#include "ProjectA/Profile/Profiler.hpp"
	#include "ProjectA/Profile/Timer.hpp"
#endif

// TODO: Replace by maths library
#include "math/math.hpp"