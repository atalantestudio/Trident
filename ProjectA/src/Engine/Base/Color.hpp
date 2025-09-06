#pragma once

namespace ProjectA {
	struct rgba {
		uint8 r;
		uint8 g;
		uint8 b;
		uint8 a;
	};

	class Color {
		public:
			// From an hexadecimal code (transparency included).
			Color(uint32 code) :
				value(
					static_cast<float32>((code >> 24) & 0xff) / 255.0f,
					static_cast<float32>((code >> 16) & 0xff) / 255.0f,
					static_cast<float32>((code >> 8) & 0xff) / 255.0f,
					static_cast<float32>(code & 0xff) / 255.0f
				)
			{}

			// From an RGB/RGBA color code.
			Color(uint8 r, uint8 g, uint8 b, uint8 a = 255) :
				value(
					static_cast<float32>(r) / 255.0f,
					static_cast<float32>(g) / 255.0f,
					static_cast<float32>(b) / 255.0f,
					static_cast<float32>(a) / 255.0f
				)
			{}

			bool operator ==(const Color& color) const {
				return value == color.value;
			}

			bool operator !=(const Color& color) const {
				return value != color.value;
			}

		public:
			// Unsigned normalized RGBA value in the range [0, 1].
			vec4<float32> value;
	};

	static void convertRedToRGBA(const sequence<uint8>& inputData, sequence<uint8>& outputData) {
		for (uint32 texelIndex = 0, channelIndex = 0; texelIndex < inputData.count; texelIndex += 1, channelIndex += 4) {
			outputData[channelIndex + 0] = inputData[texelIndex];
			outputData[channelIndex + 1] = 0;
			outputData[channelIndex + 2] = 0;
			outputData[channelIndex + 3] = 255;
		}
	}

	static void convertRGBAToRed(const sequence<uint8>& inputData, sequence<uint8>& outputData) {
		for (uint32 texelIndex = 0, channelIndex = 0; texelIndex < outputData.count; texelIndex += 1, channelIndex += 4) {
			outputData[texelIndex] = inputData[channelIndex];
		}
	}
}