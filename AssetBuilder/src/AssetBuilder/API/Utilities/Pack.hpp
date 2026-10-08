// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Base/Type/float16.hpp"

#include "AssetBuilder/API/Utilities/Utilities.hpp"

namespace Trident {
	template<>
	uint32 pack<VertexInputAttributeFormat::R10G10B10A2_UNORM>(vec3<float32> value) {
		static constexpr float32 scale = 0x3ff * 0.5f;

		// TODO(lyah): Refactor with new math API.
		return (
			static_cast<uint32>(round(clamp((value[0] + 1.0f) * scale, 0.0f, 1023.0f))) |
			(static_cast<uint32>(round(clamp((value[1] + 1.0f) * scale, 0.0f, 1023.0f))) << 10) |
			(static_cast<uint32>(round(clamp((value[2] + 1.0f) * scale, 0.0f, 1023.0f))) << 20)
		);
	}

	template<>
	uint32 pack<VertexInputAttributeFormat::R10G10B10A2_UNORM>(vec4<float32> value) {
		static constexpr float32 scale = 0x3ff * 0.5f;

		// TODO(lyah): Refactor with new math API.
		return (
			static_cast<uint32>(round(clamp((value[0] + 1.0f) * scale, 0.0f, 1023.0f))) |
			(static_cast<uint32>(round(clamp((value[1] + 1.0f) * scale, 0.0f, 1023.0f))) << 10) |
			(static_cast<uint32>(round(clamp((value[2] + 1.0f) * scale, 0.0f, 1023.0f))) << 20) |
			(static_cast<uint32>(clamp(value[3] * 0.5f + 0.5f, 0.0f, 1.0f)) << 30)
		);
	}

	template<>
	uint32 pack<VertexInputAttributeFormat::R16G16_UNORM>(vec2<float32> value) {
		static constexpr float32 scale = 0xffff;

		// TODO(lyah/clamp)
		const vec2<uint32> a = vec2<uint32>(round(clamp(value, vec2<float32>(0.0f), vec2<float32>(1.0f)) * scale));

		return a.x | (a.y << 16);
	}

	template<>
	uint32 pack<VertexInputAttributeFormat::R16G16_FLOAT>(vec2<float32> value) {
		return float16(value.x) | (float16(value.y) << 16);
	}
}