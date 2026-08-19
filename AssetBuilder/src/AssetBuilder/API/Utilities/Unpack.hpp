// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Utilities/Utilities.hpp"

namespace Trident {
	template<>
	svec3<float32> unpack<VertexInputAttributeFormat::R10G10B10A2_UNORM>(uint32 value) {
		static constexpr float32 scale = 1.0f / (0x3ff * 0.5f);

		return {
			(value & 0x3ff) * scale - 1.0f,
			((value >> 10) & 0x3ff) * scale - 1.0f,
			((value >> 20) & 0x3ff) * scale - 1.0f,
		};
	}

	template<>
	vec4<float32> unpack<VertexInputAttributeFormat::R10G10B10A2_UNORM>(uint32 value) {
		static constexpr float32 scale = 1.0f / (0x3ff * 0.5f);

		return {
			(value & 0x3ff) * scale - 1.0f,
			((value >> 10) & 0x3ff) * scale - 1.0f,
			((value >> 20) & 0x3ff) * scale - 1.0f,
			((value >> 30) & 0x3) * 2.0f - 1.0f,
		};
	}

	template<>
	svec2<float32> unpack<VertexInputAttributeFormat::R16G16_UNORM>(uint32 value) {
		static constexpr float32 scale = 1.0f / (0xffff * 0.5f);

		return {
			(value & 0xffff) * scale - 1.0f,
			((value >> 16) & 0xffff) * scale - 1.0f,
		};
	}
}