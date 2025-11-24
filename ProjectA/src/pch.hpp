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

#include "Engine/config.hpp"

#include "Engine/Base/Macros.hpp"
#include "Engine/Base/types.hpp"
#include "Engine/Base/binary.hpp"
#include "Engine/Base/cast.hpp"
#include "Engine/Base/enum.hpp"
#include "Engine/Base/Endianness.hpp"
#include "Engine/Base/UDL.hpp"

#include "Engine/Base/Platform/Platform.hpp"

#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
	#include "Engine/Base/Windows.hpp"
#endif

#ifdef ENABLE_OPENGL_ADAPTER
	#include "Backend/OpenGL/base.hpp"
#endif

#ifdef ENABLE_VULKAN_ADAPTER
	#include "Backend/Vulkan/base.hpp"
#endif

#include "Engine/Base/Random.hpp"
#include "Engine/Base/VersionNumber.hpp"

// TODO: Handle in ModuleBase?
#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
	#undef max
	#undef min
#endif

// Register modules.
#include "Engine/Module/Include/lyah.hpp"
#include "Engine/Module/Include/scroll.hpp"
#include "Engine/Module/Include/tsl.hpp"

// TODO
// The Scroll adapter must be included before Log.hpp because it includes the scroll namespace,
// but Extent.hpp is a dependency of the Lyah adapter.
#include "Engine/Module/Adapter/scroll.hpp"

#include "Engine/Base/Log.hpp"
#include "Engine/Base/Extent.hpp"
#include "Engine/Base/String.hpp"

// Register module adapters.
#include "Engine/Module/Adapter/lyah.hpp"
#include "Engine/Module/Adapter/tsl.hpp"

#include "Engine/Base/Color.hpp"
#include "Engine/Base/KeyCode.hpp"

#include "Engine/Manifest.hpp"