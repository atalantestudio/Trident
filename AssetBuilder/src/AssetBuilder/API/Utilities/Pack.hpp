// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Base/Type/float16.hpp"

#include "AssetBuilder/API/Utilities/Utilities.hpp"

namespace Trident {
	template<>
	uint32 pack<VertexInputAttributeFormat::R10G10B10A2_UNORM>(svec3<float32> value) {
		static constexpr float32 scale = 0x3ff * 0.5f;

		return (
			static_cast<uint32>(round(clamp((value[0] + 1.0f) * scale, 0.0f, 1023.0f))) |
			(static_cast<uint32>(round(clamp((value[1] + 1.0f) * scale, 0.0f, 1023.0f))) << 10) |
			(static_cast<uint32>(round(clamp((value[2] + 1.0f) * scale, 0.0f, 1023.0f))) << 20)
		);
	}

	template<>
	uint32 pack<VertexInputAttributeFormat::R10G10B10A2_UNORM>(vec4<float32> value) {
		static constexpr float32 scale = 0x3ff * 0.5f;

		return (
			static_cast<uint32>(round(clamp((value[0] + 1.0f) * scale, 0.0f, 1023.0f))) |
			(static_cast<uint32>(round(clamp((value[1] + 1.0f) * scale, 0.0f, 1023.0f))) << 10) |
			(static_cast<uint32>(round(clamp((value[2] + 1.0f) * scale, 0.0f, 1023.0f))) << 20) |
			(static_cast<uint32>(clamp(value[3] * 0.5f + 0.5f, 0.0f, 1.0f)) << 30)
		);
	}

	template<>
	uint32 pack<VertexInputAttributeFormat::R16G16_UNORM>(svec2<float32> value) {
		static constexpr float32 scale = 0xffff;

		const uint32 x = static_cast<uint32>(round(clamp(value.x, 0.0f, 1.0f) * scale));
		const uint32 y = static_cast<uint32>(round(clamp(value.y, 0.0f, 1.0f) * scale));

		return x | (y << 16);
	}

	template<>
	uint32 pack<VertexInputAttributeFormat::R16G16_FLOAT>(svec2<float32> value) {
		return float16(value.x) | (float16(value.y) << 16);
	}
}