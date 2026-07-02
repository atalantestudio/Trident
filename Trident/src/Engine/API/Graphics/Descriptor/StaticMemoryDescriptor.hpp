// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Buffer.hpp"
#include "Engine/API/Graphics/Image.hpp"

namespace Trident {
	struct StaticMemoryDescriptor {
		sequence<ref<Buffer>> buffers;
		sequence<ref<Image>> images;
	};
}