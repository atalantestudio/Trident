#pragma once

#define _USE_MATH_DEFINES

#include <algorithm>
#include <array>
#include <chrono> // Logger
#include <fstream> // std::ofstream (profile-only)
#include <functional>
#include <iomanip> // Logger
#include <iostream>
#include <math.h>
#include <memory>
#include <sstream>
#include <string>
#include <thread> // std::thread::id (profile-only)
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#ifdef WINDOWS
	#include <Windows.h>

	#undef ERROR
#endif

// TODO: Replace by maths library
#include "math/math.hpp"

#include "ProjectA/types.hpp"

#undef assert

// TODO: Debug only
#include "ProjectA/Logger/Logger.hpp"

// TODO: Profile-only
#include "ProjectA/Profile/Profiler.hpp"
#include "ProjectA/Profile/Timer.hpp"