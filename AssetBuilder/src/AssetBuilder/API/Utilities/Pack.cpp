// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Utilities/Utilities.hpp"

namespace Trident {
	template<>
	uint32 pack<VertexInputAttributeFormat::R10G10B10A2_UNORM>(svec3<float32> value) {
		static constexpr float32 scale = 0x3ff * 0.5f;

		return (
			(static_cast<uint32>((value.x + 1.0f) * scale) & 0x3ff) |
			((static_cast<uint32>((value.y + 1.0f) * scale) & 0x3ff) << 10) |
			((static_cast<uint32>((value.z + 1.0f) * scale) & 0x3ff) << 20)
		);
	}

	template<>
	uint32 pack<VertexInputAttributeFormat::R16G16_UNORM>(svec2<float32> value) {
		static constexpr float32 scale = 0xffff * 0.5f;

		return (
			static_cast<uint32>((value.x + 1.0f) * scale) |
			(static_cast<uint32>((value.y + 1.0f) * scale) << 16)
		);
	}
}