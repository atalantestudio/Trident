#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>

#ifdef BUILD_MODE_DEBUG
	#include <chrono>
	#include <iomanip>
#endif

#ifdef BUILD_MODE_PROFILE
	#include <algorithm>
	#include <thread>
#endif

#include "Engine/Base/base.hpp"