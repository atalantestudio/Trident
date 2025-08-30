#pragma once

#include "Engine/config.hpp"

#include "Engine/Base/context.hpp"
#include "Engine/Base/types.hpp"
#include "Engine/Base/binary.hpp"
#include "Engine/Base/cast.hpp"
#include "Engine/Base/enum.hpp"
#include "Engine/Base/Endianness.hpp"
#include "Engine/Base/Platform/Platform.hpp"
#include "Engine/Base/Log.hpp"
#include "Engine/Base/Macros.hpp"
#include "Engine/Base/Extent.hpp"
#include "Engine/Base/Limits.hpp"
#include "Engine/Base/UDL.hpp"

#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
	#include "Engine/Base/Windows.hpp"
#endif

#ifdef ENABLE_OPENGL_ADAPTER
	#include "Backend/OpenGL/base.hpp"
#endif

#ifdef ENABLE_VULKAN_ADAPTER
	#include "Backend/Vulkan/base.hpp"
#endif

#include "Engine/Base/sequence.hpp"
#include "Engine/Base/String.hpp"
#include "Engine/Base/Error.hpp"
#include "Engine/Base/Random.hpp"
#include "Engine/Base/VersionNumber.hpp"