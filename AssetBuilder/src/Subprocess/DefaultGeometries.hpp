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
		return {
			.vertexPositionComponents = {
				 0.5, -0.5,  0.5,
				 0.5,  0.5,  0.5,
				-0.5,  0.5,  0.5,
				-0.5, -0.5,  0.5,
				-0.5, -0.5, -0.5,
				-0.5,  0.5, -0.5,
				 0.5,  0.5, -0.5,
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
				 0,  0,  -1,
				 0,  0,  -1,
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
				0, 2, 3,
				// Back (-Z)
				4, 5, 6,
				4, 6, 7,
				// Left (-X)
				3, 2, 5,
				3, 5, 4,
				// Right (+X)
				7, 6, 1,
				7, 1, 0,
				// Top (+Y)
				5, 2, 1,
				5, 1, 6,
				// Bottom (-Y)
				3, 4, 7,
				3, 7, 0,
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
				0, 2, 3,
				0, 3, 1,
				// Back (-Z)
				0, 2, 3,
				0, 3, 1,
				// Left (-X)
				0, 2, 3,
				0, 3, 1,
				// Right (+X)
				0, 2, 3,
				0, 3, 1,
				// Top (+Y)
				0, 2, 3,
				0, 3, 1,
				// Bottom (-Y)
				0, 2, 3,
				0, 3, 1,
			},
		};
	}
}