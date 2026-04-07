// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Base.hpp"

namespace Trident {
	// TODO: Remove SET_DEBUG_NAME.
	#if defined(BUILD_MODE_DEBUG)
		#define SET_DEBUG_NAME(descriptor, value) descriptor.name = value
	#else
		#define SET_DEBUG_NAME(descriptor, value)
	#endif
}