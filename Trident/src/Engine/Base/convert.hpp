// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ImageFormat.hpp"

namespace Trident {
	// TODO(simd): Replace with SIMD math.
	/// These utilities provide unoptimized image format conversion and are not meant to be used in production builds.
	template<ImageFormat InputFormat, ImageFormat OutputFormat>
	void convert(view<uint8> inputData, uint8* outputData, uint64 outputDataCount);

	template<>
	inline void convert<ImageFormat::R8_UNORM, ImageFormat::R8G8B8A8_UNORM>(view<uint8> inputData, uint8* outputData, uint64 outputDataCount) {
		uint64 inputChannelOffset = 0;
		uint64 outputChannelOffset = 0;

		while (outputChannelOffset < outputDataCount) {
			outputData[outputChannelOffset + 0] = 255;
			outputData[outputChannelOffset + 1] = 255;
			outputData[outputChannelOffset + 2] = 255;
			outputData[outputChannelOffset + 3] = inputData[inputChannelOffset];

			inputChannelOffset += 1;
			outputChannelOffset += 4;
		}
	}

	template<>
	inline void convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(view<uint8> inputData, uint8* outputData, uint64 outputDataCount) {
		uint64 inputChannelOffset = 0;
		uint64 outputChannelOffset = 0;

		while (outputChannelOffset < outputDataCount) {
			outputData[outputChannelOffset + 0] = inputData[inputChannelOffset + 0];
			outputData[outputChannelOffset + 1] = inputData[inputChannelOffset + 1];
			outputData[outputChannelOffset + 2] = inputData[inputChannelOffset + 2];
			outputData[outputChannelOffset + 3] = 255;

			inputChannelOffset += 3;
			outputChannelOffset += 4;
		}
	}

	template<>
	inline void convert<ImageFormat::R8G8B8A8_UNORM, ImageFormat::R8_UNORM>(view<uint8> inputData, uint8* outputData, uint64 outputDataCount) {
		uint64 inputChannelOffset = 0;
		uint64 outputChannelOffset = 0;

		while (outputChannelOffset < outputDataCount) {
			outputData[outputChannelOffset + 0] = inputData[inputChannelOffset + 0];

			inputChannelOffset += 4;
			outputChannelOffset += 1;
		}
	}
}