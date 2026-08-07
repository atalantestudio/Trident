// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct GeometryIndex3D {
		bool operator==(GeometryIndex3D other) const {
			return (
				positionIndex == other.positionIndex &&
				normalIndex == other.normalIndex &&
				textureCoordinateIndex == other.textureCoordinateIndex
			);
		}

		uint32 positionIndex;
		uint32 normalIndex;
		uint32 textureCoordinateIndex;
	};
}

template<>
struct std::hash<Trident::GeometryIndex3D> {
	/// [  (4 bits)  |  Texture coordinate (20 bits)  |  Normal (20 bits)  |  Position (20 bits)  ]
	std::size_t operator()(Trident::GeometryIndex3D key) const {
		return (
			static_cast<std::size_t>(key.positionIndex & 0xfffff) |
			(static_cast<std::size_t>(key.normalIndex & 0xfffff) << 20) |
			(static_cast<std::size_t>(key.textureCoordinateIndex & 0xfffff) << 40)
		);
	}
};