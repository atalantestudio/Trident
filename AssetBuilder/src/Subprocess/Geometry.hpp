// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"

namespace Trident {
	/// Creates and returns a 2D geometry representing a unit circle.
	Geometry2D createUnitCircleGeometry(uint32 resolution, std::vector<float32>& vertexComponentBuffer, std::vector<uint32>& indexBuffer) {
		ENGINE_ASSERT(resolution > 2, "The resolution must be greater than 2.");

		constexpr float32 radius = 0.5_m;

		const uint32 vertexComponentOffset = static_cast<uint32>(vertexComponentBuffer.size());
		const uint32 indexOffset = static_cast<uint32>(indexBuffer.size());
		const float32 theta = pi<float32>() * 2.0f / resolution;

		vertexComponentBuffer.resize(vertexComponentBuffer.size() + resolution * 4);
		indexBuffer.resize(indexBuffer.size() + resolution * 3);

		uint32 triangleIndex = 0;
		float32 angle = 0.0f;
		vec2<float32> vertex;

		while (triangleIndex < resolution) {
			vertex = {sin(angle), cos(angle)};
			vertex *= radius;

			vertexComponentBuffer[vertexComponentOffset + triangleIndex * 2 + 0] = vertex[0];
			vertexComponentBuffer[vertexComponentOffset + triangleIndex * 2 + 1] = vertex[1];

			// TODO: Generate vertex texture coordinates.

			indexBuffer[indexOffset + triangleIndex * 3 + 0] = 0;
			indexBuffer[indexOffset + triangleIndex * 3 + 1] = triangleIndex + 1;
			indexBuffer[indexOffset + triangleIndex * 3 + 2] = triangleIndex + 2;

			angle += theta;
			triangleIndex += 1;
		}

		return Geometry2D({
			.vertexPositionComponentRange = {vertexComponentOffset, resolution * 2},
			.vertexTextureCoordinateComponentRange = {vertexComponentOffset + resolution * 2, resolution * 2},
			.indexRange = {indexOffset, resolution * 3},
		});
	}

	/// Creates and returns a 2D geometry representing a unit square.
	/// 
	///         0
	///    0 ‒‒‒‒‒‒‒ 1
	///    ¦         ¦
	/// 3  ¦         ¦  1
	///    ¦         ¦
	///    3 ‒‒‒‒‒‒‒ 2
	///         2
	/// 
	Geometry2D createUnitSquareGeometry(std::vector<float32>& vertexComponentBuffer, std::vector<uint32>& indexBuffer) {
		const uint32 vertexComponentOffset = static_cast<uint32>(vertexComponentBuffer.size());
		const uint32 indexOffset = static_cast<uint32>(indexBuffer.size());

		vertexComponentBuffer.insert(vertexComponentBuffer.end(), {
			// Positions
			-0.5f,  0.5f, // Top left
			 0.5f,  0.5f, // Top right
 			 0.5f, -0.5f, // Bottom right
			-0.5f, -0.5f, // Bottom left

			// Texture coordinates
			0.0f, 0.0f, // Top left
			1.0f, 0.0f, // Top right
			1.0f, 1.0f, // Bottom right
			0.0f, 1.0f, // Bottom left
		});

		indexBuffer.insert(indexBuffer.end(), {
			0, 1, 2,
			0, 2, 3,
		});

		return Geometry2D({
			.vertexPositionComponentRange = {vertexComponentOffset, 8},
			.vertexTextureCoordinateComponentRange = {vertexComponentOffset + 8, 8},
			.indexRange = {indexOffset, 6},
		});
	}
}