// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct TextureArrayLayer {
		Extent2D extent;
		sequence<uint8> data;
	};
}