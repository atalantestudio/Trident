#pragma once

#define _USE_MATH_DEFINES

#include <algorithm>
#include <functional>
#include <iostream>
#include <math.h>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "ProjectA/Logger.hpp"

// TODO: Profile-only PCH header?
#include "ProjectA/Profile/Profiler.hpp"
#include "ProjectA/Profile/Timer.hpp"

#ifdef WINDOWS
	#include <Windows.h>
#endif