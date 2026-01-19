#pragma once

namespace ProjectA {
	struct TextureArrayLayer {
		uint32 width;
		uint32 height;
		sequence<uint8> data;
	};
}