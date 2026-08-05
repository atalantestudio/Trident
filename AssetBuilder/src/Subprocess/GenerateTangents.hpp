// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Index.hpp"

namespace Trident {
	/// Computes and returns the vertex tangents of a geometry.
	sequence<svec3<float32>> generateTangents(view<svec3<float32>> positions, view<svec3<float32>> normals, view<svec2<float32>> textureCoordinates, view<GeometryIndex3D> vertices);
}