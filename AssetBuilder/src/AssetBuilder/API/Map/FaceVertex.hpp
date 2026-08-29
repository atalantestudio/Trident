// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct FaceVertex {
		svec3<float32> position;

		// TODO(asset-builder/map/face-vertex): Use.
		svec2<float32> textureCoordinate;
	};
}