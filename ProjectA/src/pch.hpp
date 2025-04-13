#pragma once

#include <array>
#include <bitset>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream> // Release build can need std::cout
#include <sstream>

#ifdef BUILD_MODE_DEBUG
	#include <chrono>
	#include <iomanip>
#endif

#ifdef BUILD_MODE_PROFILE
	#include <algorithm>
	#include <thread>
#endif

#include "Engine/base.hpp"

#include "Engine/lyah.hpp"

#include "Engine/types.hpp"
#include "Engine/fchain.hpp"

#ifdef BUILD_MODE_DEBUG
	#include "Engine/Logger/Logger.hpp"
#endif

#ifdef BUILD_MODE_PROFILE
	#include "Engine/Profile/Profiler.hpp"
	#include "Engine/Profile/Timer.hpp"

	#define PROFILER(path) ::ProjectA::Profiler profiler = ::ProjectA::Profiler(path)

	#define PROFILE_SCOPE(name) ::ProjectA::Timer timer = ::ProjectA::Timer(name)
#endif

#include "Engine/to_string.hpp"