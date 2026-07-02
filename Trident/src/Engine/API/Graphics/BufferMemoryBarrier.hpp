// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Access.hpp"
#include "Engine/API/Graphics/Buffer.hpp"

namespace Trident {
	struct BufferMemoryBarrier {
		ref<Buffer> buffer;
		uint64 offset;
		uint64 size;
		Access newAccess;
	};
}