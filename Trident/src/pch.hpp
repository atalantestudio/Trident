// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

// TODO: Conditionally import per module.
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>

// Used by the physics engine.
#include <map>

#include <random>
#include <sstream>

// TODO: Conditionally import per module.
#if defined(BUILD_MODE_DEBUG)
	#include <chrono>
	#include <iomanip>
#endif

// TODO: Conditionally import per module.
#if defined(BUILD_MODE_PROFILE)
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

#if defined(ENABLE_DX12_ADAPTER)
	#include "Engine/Adapter/Graphics/DX12/Base.hpp"
#endif

#if defined(ENABLE_GL_ADAPTER)
	#include "Engine/Adapter/Graphics/GL/Base.hpp"
#endif

#if defined(ENABLE_VK_ADAPTER)
	#include "Engine/Adapter/Graphics/VK/Base.hpp"
#endif

#include "Engine/Base/Random.hpp"
#include "Engine/Base/VersionNumber.hpp"

#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
	#undef max
	#undef min

	#if STANDARD >= STANDARD_CPP20
		#undef far
		#undef near
	#endif
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
#include "Engine/Base/convert.hpp"
#include "Engine/Base/KeyCode.hpp"
#include "Engine/Base/pool.hpp"
#include "Engine/Base/Transform.hpp"

#include "Engine/Manifest.hpp"

#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
	#include "Engine/Adapter/Window/Win32/base.hpp"
#endif