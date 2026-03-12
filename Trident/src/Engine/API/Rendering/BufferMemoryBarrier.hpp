// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Access.hpp"
#include "Engine/API/Rendering/Buffer.hpp"

namespace Trident {
	struct BufferMemoryBarrier {
		ref<BaseBuffer> buffer;
		uint64 offset;
		uint64 size;

		// TODO: Store in the buffer.
		Access sourceAccess;

		Access destinationAccess;
	};
}