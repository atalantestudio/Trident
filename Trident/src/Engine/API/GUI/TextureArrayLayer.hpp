// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct TextureArrayLayer {
		uint32 width;
		uint32 height;
		sequence<uint8> data;
	};
}