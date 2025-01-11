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

#ifdef WINDOWS
	#include <Windows.h>

	#undef ERROR
#endif

#include "ProjectA/Logger.hpp"

// TODO: Replace by maths library
#include "mat/mat4x4.hpp"
#include "vec/vec2.hpp"
#include "vec/vec3.hpp"
#include "vec/vec4.hpp"

// TODO: Profile-only PCH header?
#include "ProjectA/Profile/Profiler.hpp"
#include "ProjectA/Profile/Timer.hpp"