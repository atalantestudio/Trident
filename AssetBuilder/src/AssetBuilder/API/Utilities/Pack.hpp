// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Base/Type/float16.hpp"

#include "AssetBuilder/API/Utilities/Utilities.hpp"

namespace Trident {
	template<>
	uint32 pack<VertexInputAttributeFormat::R10G10B10A2_UNORM>(vec3<float32> value) {
		static constexpr float32 scale = 0x3ff * 0.5f;

		// TODO(lyah/clamp)
		const vec3<uint32> rgb = vec3<uint32>(round(clamp((value + vec3<float32>(1.0f)) * scale, vec3<float32>(0.0f), vec3<float32>(1023.0f))));

		return (rgb.z << 20) | (rgb.y << 10) | rgb.x;
	}

	template<>
	uint32 pack<VertexInputAttributeFormat::R10G10B10A2_UNORM>(vec4<float32> value) {
		static constexpr float32 scale = 0x3ff * 0.5f;

		const vec3<float32> a = {value.x, value.y, value.z};
		const vec3<uint32> rgb = vec3<uint32> (round(clamp((a + vec3<float32>(1.0f)) * scale, vec3<float32>(0.0f), vec3<float32>(1023.0f))));
		const uint32 w = static_cast<uint32>(clamp(value.w * 0.5f + 0.5f, 0.0f, 1.0f));

		return (w << 30) | (rgb.z << 20) | (rgb.y << 10) | rgb.x;
	}

	template<>
	uint32 pack<VertexInputAttributeFormat::R16G16_UNORM>(vec2<float32> value) {
		static constexpr float32 scale = 0xffff;

		// TODO(lyah/clamp)
		const vec2<uint32> rg = vec2<uint32>(round(clamp(value, vec2<float32>(0.0f), vec2<float32>(1.0f)) * scale));

		return (rg.y << 16) | rg.x;
	}

	template<>
	uint32 pack<VertexInputAttributeFormat::R16G16_FLOAT>(vec2<float32> value) {
		return (float16(value.y) << 16) | float16(value.x);
	}
}