#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>

#ifdef BUILD_MODE_DEBUG
	#include <chrono>
	#include <iomanip>
#endif

#ifdef BUILD_MODE_PROFILE
	#include <algorithm>
	#include <thread>
#endif

// Register modules.
#include "Engine/Module/Include/lyah.hpp"

#include "Engine/Base/base.hpp"

// Register module adapters.
#include "Engine/Module/Adapter/lyah.hpp"

#include "Engine/Base/Color.hpp"

#include "Engine/Manifest.hpp"