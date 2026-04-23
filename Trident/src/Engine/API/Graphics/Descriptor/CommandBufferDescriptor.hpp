// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/CommandBufferUsage.hpp"

namespace Trident {
	struct CommandBufferDescriptor {
		CommandBufferUsage usage;

		#if defined(DEBUG_OBJECT_NAME)
			view<char8> name;
		#endif
	};
}