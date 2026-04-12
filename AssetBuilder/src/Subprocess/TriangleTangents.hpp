// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	/// Computes and returns the triangle tangents of a geometry.
	sequence<float32> calculateTriangleTangentComponents(
		view<float64> vertexPositionComponents,
		view<float64> vertexTextureCoordinateComponents,
		view<uint32> vertexPositionIndices,
		view<uint32> vertexTextureCoordinateIndices
	);
}