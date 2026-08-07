// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	/// See https://paroj.github.io/gltut/Optimize%20Vertex%20Format.html
	/// and https://stackoverflow.com/a/49460793/17136841.
	struct Vertex3D {
		// TODO(geometry/vertex-compression): Replace with 3 16-bit signed normalized shorts.
		/// Stored as R32G32B32_SFLOAT.
		svec3<float32> position;

		/// Stored as R10G10B10A2_UNORM.
		/// R10G10B10A2_SNORM would be better but it's not supported on DX12.
		uint32 normal;

		/// Stored as R10G10B10A2_UNORM.
		/// R10G10B10A2_SNORM would be better but it's not supported on DX12.
		uint32 tangent;

		// TODO(geometry/vertex-compression): Use R16G16_FLOAT instead. Values can go outside of [0, 1].
		/// Stored as R16G16_UNORM.
		uint32 textureCoordinate;
	};
}