// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Geometry.hpp"
#include "Hull.hpp"
#include "TriangleTangents.hpp"

namespace Trident {
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

	Geometry2DData createUnitSquare() {
		std::vector<float32> vertexComponentBuffer;
		std::vector<uint32> indexBuffer;

		const Geometry2D geometry = createUnitSquareGeometry(vertexComponentBuffer, indexBuffer);
		const Hull2D hull = createHull<Hull2DType::POLYGON>(geometry, vertexComponentBuffer, indexBuffer);

		return {
			.vertexComponentBuffer = vertexComponentBuffer,
			.indexBuffer = indexBuffer,
			.geometry = geometry,
			.hull = hull,
		};
	}

	Geometry3DData createUnitBox() {
		Geometry3DData data = {
			.vertexPositionComponents = {
				 0.5,  0.5,  0.5,
				-0.5,  0.5,  0.5,
				 0.5, -0.5,  0.5,
				-0.5, -0.5,  0.5,
				-0.5,  0.5, -0.5,
				 0.5,  0.5, -0.5,
				-0.5, -0.5, -0.5,
				 0.5, -0.5, -0.5,
			},
			.vertexNormalComponents = {
				 0.0,  0.0,  1.0,
				 0.0,  0.0, -1.0,
				-1.0,  0.0,  0.0,
				 1.0,  0.0,  0.0,
				 0.0,  1.0,  0.0,
				 0.0, -1.0,  0.0,
			},
			.vertexTextureCoordinateComponents = {
				0.0, 0.0,
				1.0, 0.0,
				0.0, 1.0,
				1.0, 1.0,
			},
			.triangleTangentComponents = {
				// Front (+Z)
				-1,  0,  0,
				-1,  0,  0,
				// Back (-Z)
				 1,  0,  0,
				 1,  0,  0,
				// Left (-X)
				 0,  0, -1,
				 0,  0, -1,
				// Right (+X)
				 0,  0,  1,
				 0,  0,  1,
				// Top (+Y)
				 1,  0,  0,
				 1,  0,  0,
				// Bottom (-Y)
				 1,  0,  0,
				 1,  0,  0,
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
				0, 1, 2,
				1, 3, 2,
				// Left (-X)
				0, 1, 2,
				1, 3, 2,
				// Right (+X)
				0, 1, 2,
				1, 3, 2,
				// Top (+Y)
				0, 1, 2,
				1, 3, 2,
				// Bottom (-Y)
				0, 1, 2,
				1, 3, 2,
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
			.hullFaceNormalComponents = {
				// NOTE: Same as the face normals (special case).
				 0.0f,  0.0f,  1.0f,
				 0.0f,  0.0f, -1.0f,
				-1.0f,  0.0f,  0.0f,
				 1.0f,  0.0f,  0.0f,
				 0.0f,  1.0f,  0.0f,
				 0.0f, -1.0f,  0.0f,
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
		};

		data.hull = createHull<Hull3DType::POLYGON>(data.vertexPositionComponents);

		return data;
	}
}