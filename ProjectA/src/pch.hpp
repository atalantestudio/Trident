#pragma once

// TODO: Conditionally import per module.
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>

// TODO: Conditionally import per module.
#ifdef BUILD_MODE_DEBUG
	#include <chrono>
	#include <iomanip>
#endif

// TODO: Conditionally import per module.
#ifdef BUILD_MODE_PROFILE
	#include <algorithm>
	#include <thread>
#endif

#include "Engine/Base/base.hpp"

// TODO: Handle in ModuleBase?
#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
	#undef max
	#undef min
#endif

// Register modules.
#include "Engine/Module/Include/lyah.hpp"
#include "Engine/Module/Include/scroll.hpp"

#include "Engine/Base/Log.hpp"
#include "Engine/Base/Extent.hpp"
#include "Engine/Base/String.hpp"

// Register module adapters.
#include "Engine/Module/Adapter/lyah.hpp"
#include "Engine/Module/Adapter/scroll.hpp"

#include "Engine/Base/Color.hpp"
#include "Engine/Base/KeyCode.hpp"

#include "Engine/Manifest.hpp"