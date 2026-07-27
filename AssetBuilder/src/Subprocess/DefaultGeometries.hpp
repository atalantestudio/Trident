// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"

#include "Hull.hpp"
#include "TriangleTangents.hpp"

namespace Trident {
	/// Creates and returns a 2D geometry representing a unit circle.
	static Geometry2D createUnitCircleGeometry(uint32 resolution, std::vector<float32>& vertexComponentBuffer, std::vector<uint32>& indexBuffer) {
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

	Geometry2DData createUnitCircle() {
		std::vector<float32> vertexComponentBuffer;
		std::vector<uint32> indexBuffer;

		const Geometry2D geometry = createUnitCircleGeometry(32, vertexComponentBuffer, indexBuffer);
		const Hull2D hull = createHull<Hull2DType::CIRCLE>(geometry, vertexComponentBuffer, indexBuffer);

		return {
			.vertexComponentBuffer = vertexComponentBuffer,
			.indexBuffer = indexBuffer,
			.geometry = geometry,
			.hull = hull,
		};
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
	Geometry2DData createUnitSquare() {
		std::vector<float32> vertexComponentBuffer = {
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
		};

		std::vector<uint32> indexBuffer = {
			0, 1, 2,
			0, 2, 3,
		};

		const Geometry2D geometry = Geometry2D({
			.vertexPositionComponentRange = {0, 8},
			.vertexTextureCoordinateComponentRange = {8, 8},
			.indexRange = {0, 6},
		});

		const Hull2D hull = createHull<Hull2DType::POLYGON>(geometry, vertexComponentBuffer, indexBuffer);

		return {
			.vertexComponentBuffer = vertexComponentBuffer,
			.indexBuffer = indexBuffer,
			.geometry = geometry,
			.hull = hull,
		};
	}

	Geometry2DData createTriangle() {
		std::vector<float32> vertexComponentBuffer = {
			// Positions
			0.0f, 0.0f,
			0.0f, 1.0f,
			6.0f, 0.0f,

			// Texture coordinates
			0.0f, 0.0f, // Top left
			0.0f, 1.0f, // Bottom left
			1.0f, 1.0f, // Bottom right
		};

		std::vector<uint32> indexBuffer = {
			0, 1, 2,
		};

		const Geometry2D geometry = Geometry2D({
			.vertexPositionComponentRange = {0, 6},
			.vertexTextureCoordinateComponentRange = {6, 6},
			.indexRange = {0, 3},
		});

		const Hull2D hull = createHull<Hull2DType::POLYGON>(geometry, vertexComponentBuffer, indexBuffer);

		return {
			.vertexComponentBuffer = vertexComponentBuffer,
			.indexBuffer = indexBuffer,
			.geometry = geometry,
			.hull = hull,
		};
	}

	Geometry2DData createPolygon() {
		std::vector<float32> vertexComponentBuffer = {
			// Positions
			-3.0f,   0.0f,
			 4.0f,   0.0f,
			 3.0f,  -1.0f,
			-2.25f, -1.5f,

			// Texture coordinates
			0.0f, 0.0f, // Top left
			1.0f, 0.0f, // Top right
			0.0f, 1.0f, // Bottom left
			1.0f, 1.0f, // Bottom right
		};

		std::vector<uint32> indexBuffer = {
			0, 1, 2,
			0, 2, 3,
		};

		const Geometry2D geometry = Geometry2D({
			.vertexPositionComponentRange = {0, 8},
			.vertexTextureCoordinateComponentRange = {8, 8},
			.indexRange = {0, 6},
		});

		const Hull2D hull = createHull<Hull2DType::POLYGON>(geometry, vertexComponentBuffer, indexBuffer);

		return {
			.vertexComponentBuffer = vertexComponentBuffer,
			.indexBuffer = indexBuffer,
			.geometry = geometry,
			.hull = hull,
		};
	}

	static Geometry3DData createPlaneGeometry(vec2<float32> scale) {
		const float32 x = scale[0];
		const float32 z = scale[1];

		Geometry3DData data = {
			.vertexPositionComponents = {
				-0.5f * x,  0.0f,  0.5f * z,
				 0.5f * x,  0.0f,  0.5f * z,
				-0.5f * x,  0.0f, -0.5f * z,
				 0.5f * x,  0.0f, -0.5f * z,
			},
			.vertexNormalComponents = {
				 0.0f,  1.0f,  0.0f,
			},
			.vertexTextureCoordinateComponents = {
				0.0f, 0.0f,
				1.0f, 0.0f,
				0.0f, 1.0f,
				1.0f, 1.0f,
			},
			.triangleTangentComponents = {
				 1.0f,  0.0f,  0.0f,
				 1.0f,  0.0f,  0.0f,
			},
			.vertexPositionIndices = {
				0, 1, 2,
				1, 3, 2,
			},
			.vertexNormalIndices = {
				0, 0, 0,
				0, 0, 0,
			},
			.vertexTextureCoordinateIndices = {
				0, 1, 2,
				1, 3, 2,
			},
			.flags = GeometryAssetFlags::HAS_HULL,
			.hullLineIndices = {
				0, 1,
				1, 3,
				3, 2,
				2, 0,
			},
			.hullEdges = {
				{.originVertexIndex = 0, .faceIndex = 0},
				{.originVertexIndex = 1, .faceIndex = 1},
				{.originVertexIndex = 1, .faceIndex = 0},
				{.originVertexIndex = 3, .faceIndex = 1},
				{.originVertexIndex = 3, .faceIndex = 0},
				{.originVertexIndex = 2, .faceIndex = 1},
				{.originVertexIndex = 2, .faceIndex = 0},
				{.originVertexIndex = 0, .faceIndex = 1},
			},
			.hullFaceEdgeIndices = {
				0, 2, 4, 6,
				1, 3, 5, 7,
			},
			.hullFaces = {
				{{ 0.0f,  1.0f,  0.0f}, 0.0f, {0, 4}},
				{{ 0.0f, -1.0f,  0.0f}, 0.0f, {4, 4}},
			},
		};

		data.hull = createHull<Hull3DType::POLYGON>(data.vertexPositionComponents);

		return data;
	}

	static Geometry3DData createBoxGeometry(vec3<float32> scale) {
		const float32 x = scale[0];
		const float32 y = scale[1];
		const float32 z = scale[2];

		Geometry3DData data = {
			.vertexPositionComponents = {
				 0.5f * x,  0.5f * y,  0.5f * z,
				-0.5f * x,  0.5f * y,  0.5f * z,
				 0.5f * x, -0.5f * y,  0.5f * z,
				-0.5f * x, -0.5f * y,  0.5f * z,
				-0.5f * x,  0.5f * y, -0.5f * z,
				 0.5f * x,  0.5f * y, -0.5f * z,
				-0.5f * x, -0.5f * y, -0.5f * z,
				 0.5f * x, -0.5f * y, -0.5f * z,
			},
			.vertexNormalComponents = {
				 0.0f,  0.0f,  1.0f,
				 0.0f,  0.0f, -1.0f,
				-1.0f,  0.0f,  0.0f,
				 1.0f,  0.0f,  0.0f,
				 0.0f,  1.0f,  0.0f,
				 0.0f, -1.0f,  0.0f,
			},
			.vertexTextureCoordinateComponents = {
				0.0f, 0.0f,
				0.25f, 0.0f,
				0.0f, 0.25f,
				0.25f, 0.25f,

				0.25f, 0.0f,
				0.5f, 0.0f,
				0.25f, 0.25f,
				0.5f, 0.25f,

				0.5f, 0.0f,
				0.75f, 0.0f,
				0.5f, 0.25f,
				0.75f, 0.25f,

				0.0f, 0.25f,
				0.25f, 0.25f,
				0.0f, 0.5f,
				0.25f, 0.5f,

				0.25f, 0.25f,
				0.5f, 0.25f,
				0.25f, 0.5f,
				0.5f, 0.5f,

				0.5f, 0.25f,
				0.75f, 0.25f,
				0.5f, 0.5f,
				0.75f, 0.5f,
			},
			.triangleTangentComponents = {
				// Front (+Z)
				-1.0f,  0.0f,  0.0f,
				-1.0f,  0.0f,  0.0f,
				// Back (-Z)
				 1.0f,  0.0f,  0.0f,
				 1.0f,  0.0f,  0.0f,
				// Left (-X)
				 0.0f,  0.0f, -1.0f,
				 0.0f,  0.0f, -1.0f,
				// Right (+X)
				 0.0f,  0.0f,  1.0f,
				 0.0f,  0.0f,  1.0f,
				// Top (+Y)
				 1.0f,  0.0f,  0.0f,
				 1.0f,  0.0f,  0.0f,
				// Bottom (-Y)
				 1.0f,  0.0f,  0.0f,
				 1.0f,  0.0f,  0.0f,
			},
			.vertexPositionIndices = {
				// Front (+Z)
				0, 1, 2,
				1, 3, 2,
				// Back (-Z)
				4, 5, 6,
				5, 7, 6,
				// Left (-X)
				1, 4, 3,
				4, 6, 3,
				// Right (+X)
				5, 0, 7,
				0, 2, 7,
				// Top (+Y)
				5, 4, 0,
				4, 1, 0,
				// Bottom (-Y)
				2, 3, 7,
				3, 6, 7,
			},
			.vertexNormalIndices = {
				// Front (+Z)
				0, 0, 0,
				0, 0, 0,
				// Back (-Z)
				1, 1, 1,
				1, 1, 1,
				// Left (-X)
				2, 2, 2,
				2, 2, 2,
				// Right (+X)
				3, 3, 3,
				3, 3, 3,
				// Top (+Y)
				4, 4, 4,
				4, 4, 4,
				// Bottom (-Y)
				5, 5, 5,
				5, 5, 5,
			},
			.vertexTextureCoordinateIndices = {
				// Front (+Z)
				0, 1, 2,
				1, 3, 2,
				// Back (-Z)
				4, 5, 6,
				5, 7, 6,
				// Left (-X)
				8, 9, 10,
				9, 11, 10,
				// Right (+X)
				12, 13, 14,
				13, 15, 14,
				// Top (+Y)
				16, 17, 18,
				17, 19, 18,
				// Bottom (-Y)
				20, 21, 22,
				21, 23, 22,
			},
			.flags = GeometryAssetFlags::HAS_HULL,
			.hullLineIndices = {
				0, 1,
				1, 3,
				3, 2,
				2, 0,
				4, 5,
				5, 7,
				7, 6,
				6, 4,
				0, 5,
				4, 1,
				7, 2,
				3, 6,
			},
			.hullEdges = {
				{.originVertexIndex = 0, .faceIndex = 0}, // front
				{.originVertexIndex = 1, .faceIndex = 4}, // top
				{.originVertexIndex = 1, .faceIndex = 0}, // front
				{.originVertexIndex = 3, .faceIndex = 2}, // left
				{.originVertexIndex = 3, .faceIndex = 0}, // front
				{.originVertexIndex = 2, .faceIndex = 5}, // bottom
				{.originVertexIndex = 2, .faceIndex = 0}, // front
				{.originVertexIndex = 0, .faceIndex = 3}, // right
				{.originVertexIndex = 4, .faceIndex = 1}, // back
				{.originVertexIndex = 5, .faceIndex = 4}, // top
				{.originVertexIndex = 5, .faceIndex = 1}, // back
				{.originVertexIndex = 7, .faceIndex = 3}, // right
				{.originVertexIndex = 7, .faceIndex = 1}, // back
				{.originVertexIndex = 6, .faceIndex = 5}, // bottom
				{.originVertexIndex = 6, .faceIndex = 1}, // back
				{.originVertexIndex = 4, .faceIndex = 2}, // left
				{.originVertexIndex = 0, .faceIndex = 4}, // top
				{.originVertexIndex = 5, .faceIndex = 3}, // right
				{.originVertexIndex = 4, .faceIndex = 4}, // top
				{.originVertexIndex = 1, .faceIndex = 2}, // left
				{.originVertexIndex = 7, .faceIndex = 5}, // bottom
				{.originVertexIndex = 2, .faceIndex = 3}, // right
				{.originVertexIndex = 3, .faceIndex = 5}, // bottom
				{.originVertexIndex = 6, .faceIndex = 2}, // left
			},
			.hullFaceEdgeIndices = {
				0, 2, 4, 6,
				8, 10, 12, 14,
				19, 15, 23, 3,
				17, 7, 21, 11,
				9, 18, 1, 16,
				5, 22, 13, 20,
			},
			.hullFaces = {
				{{ 0.0f,  0.0f,  1.0f}, z * 0.5f, {0, 4}},
				{{ 0.0f,  0.0f, -1.0f}, z * 0.5f, {4, 4}},
				{{-1.0f,  0.0f,  0.0f}, x * 0.5f, {8, 4}},
				{{ 1.0f,  0.0f,  0.0f}, x * 0.5f, {12, 4}},
				{{ 0.0f,  1.0f,  0.0f}, y * 0.5f, {16, 4}},
				{{ 0.0f, -1.0f,  0.0f}, y * 0.5f, {20, 4}},
			},
		};

		data.hull = createHull<Hull3DType::POLYGON>(data.vertexPositionComponents);

		return data;
	}

	Geometry3DData createUnitBox() {
		return createBoxGeometry({1.0f, 1.0f, 1.0f});
	}

	Geometry3DData createUnitPlane() {
		return createPlaneGeometry({1.0f, 1.0f});
	}

	Geometry3DData createRectangularCuboid() {
		return createBoxGeometry({2.0f, 0.5f, 2.0f});
	}
}