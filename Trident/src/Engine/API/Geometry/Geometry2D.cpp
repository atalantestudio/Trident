// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Geometry/Geometry2D.hpp"

namespace Trident {
	Geometry2D Geometry2D::unitCircle(uint32 resolution) {
		ENGINE_ASSERT(resolution > 2, "The resolution must be greater than 2.");

		const float32 radius = 0.5_m;
		const float32 theta = pi<float32>() * 2.0f / resolution;

		sequence<svec2<float32>> vertices = sequence<svec2<float32>>(resolution);
		sequence<uint16> fillIndices = sequence<uint16>(resolution * 3);

		// TODO: Restore line indices?
		// sequence<uint16> lineIndices = sequence<uint16>(resolution + 1);

		float32 angle = 0.0f;
		uint32 triangleIndex = 0;
		vec2<float32> vertex;

		while (triangleIndex < resolution) {
			vertex = {sin(angle), cos(angle)};
			vertex *= radius;

			vertices[triangleIndex * 2] = {vertex[0], vertex[1]};

			fillIndices[triangleIndex * 3 + 0] = 0;
			fillIndices[triangleIndex * 3 + 1] = triangleIndex + 1;
			fillIndices[triangleIndex * 3 + 2] = triangleIndex + 2;

			// TODO: Restore line indices?
			// lineIndices[triangleIndex] = triangleIndex;

			angle += theta;
			triangleIndex += 1;
		}

		// TODO: Restore line indices?
		// lineIndices[triangleIndex] = 0;

		return Geometry2D({
			.vertices = vertices,
			.indices = fillIndices,
		});
	}

	Geometry2D Geometry2D::unitSquare() {
		return Geometry2D({
			.vertices = {
				{-0.5f,  0.5f}, // Top left
				{ 0.5f,  0.5f}, // Top right
				{ 0.5f, -0.5f}, // Bottom right
				{-0.5f, -0.5f}, // Bottom left
			},
			.indices = {
				0, 1, 2,
				0, 2, 3,
			},
		});
	}

	Geometry2D::Geometry2D(const Descriptor<Geometry2D>& descriptor) :
		vertices(descriptor.vertices),
		indices(descriptor.indices)
	{}
}