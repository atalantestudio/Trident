// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class Color {
		public:
			// From an hexadecimal code (transparency included).
			constexpr Color(uint32 code) :
				value(
					static_cast<float32>((code >> 24) & 0xff) / 255.0f,
					static_cast<float32>((code >> 16) & 0xff) / 255.0f,
					static_cast<float32>((code >> 8) & 0xff) / 255.0f,
					static_cast<float32>(code & 0xff) / 255.0f
				)
			{}

			// From an RGB/RGBA color code.
			constexpr Color(uint8 r, uint8 g, uint8 b, uint8 a = 255) :
				value(
					static_cast<float32>(r) / 255.0f,
					static_cast<float32>(g) / 255.0f,
					static_cast<float32>(b) / 255.0f,
					static_cast<float32>(a) / 255.0f
				)
			{}

			inline bool operator==(const Color& color) const {
				return value == color.value;
			}

			inline bool operator!=(const Color& color) const {
				return value != color.value;
			}

		public:
			// Unsigned normalized RGBA value in the range [0, 1].
			vec4<float32> value;
	};
}