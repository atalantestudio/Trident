// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Buffer.hpp"
#include "Engine/API/Graphics/Image.hpp"

namespace Trident {
	struct StaticMemoryDescriptor {
		/// Buffers that are statically allocated.
		view<ref<Buffer>> buffers;

		/// Images that are statically allocated.
		view<ref<Image>> images;
	};
}