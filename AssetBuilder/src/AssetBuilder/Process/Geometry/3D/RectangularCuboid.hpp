// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Geometry/Geometry3DProcess.hpp"

namespace Trident {
	class RectangularCuboidGeometry3D final : public Geometry3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_rectangular_cuboid";
			}

			BuildGeometry3D createGeometry() const final override {
				const float32 x = 2.0f;
				const float32 y = 0.5f;
				const float32 z = 2.0f;

				BuildGeometry3D geometry = {
					.flags = GeometryAssetFlags::IS_3D | GeometryAssetFlags::HAS_HULL,
					.vertices = {
						// Front (+Z)
						{{ 0.5f * x,  0.5f * y,  0.5f * z}, { 0.0f,  0.0f,  1.0f}, {-1.0f,  0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
						{{-0.5f * x,  0.5f * y,  0.5f * z}, { 0.0f,  0.0f,  1.0f}, {-1.0f,  0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
						{{ 0.5f * x, -0.5f * y,  0.5f * z}, { 0.0f,  0.0f,  1.0f}, {-1.0f,  0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},
						{{-0.5f * x, -0.5f * y,  0.5f * z}, { 0.0f,  0.0f,  1.0f}, {-1.0f,  0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},

						// Back (-Z)
						{{-0.5f * x,  0.5f * y, -0.5f * z}, { 0.0f,  0.0f, -1.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
						{{ 0.5f * x,  0.5f * y, -0.5f * z}, { 0.0f,  0.0f, -1.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
						{{-0.5f * x, -0.5f * y, -0.5f * z}, { 0.0f,  0.0f, -1.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},
						{{ 0.5f * x, -0.5f * y, -0.5f * z}, { 0.0f,  0.0f, -1.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},

						// Left (-X)
						{{-0.5f * x,  0.5f * y,  0.5f * z}, {-1.0f,  0.0f,  0.0f}, { 0.0f,  0.0f, -1.0f,  1.0f}, {0.0f, 0.0f}},
						{{-0.5f * x,  0.5f * y, -0.5f * z}, {-1.0f,  0.0f,  0.0f}, { 0.0f,  0.0f, -1.0f,  1.0f}, {1.0f, 0.0f}},
						{{-0.5f * x, -0.5f * y,  0.5f * z}, {-1.0f,  0.0f,  0.0f}, { 0.0f,  0.0f, -1.0f,  1.0f}, {0.0f, 1.0f}},
						{{-0.5f * x, -0.5f * y, -0.5f * z}, {-1.0f,  0.0f,  0.0f}, { 0.0f,  0.0f, -1.0f,  1.0f}, {1.0f, 1.0f}},

						// Right (+X)
						{{ 0.5f * x,  0.5f * y, -0.5f * z}, { 1.0f,  0.0f,  0.0f}, { 0.0f,  0.0f,  1.0f,  1.0f}, {0.0f, 0.0f}},
						{{ 0.5f * x,  0.5f * y,  0.5f * z}, { 1.0f,  0.0f,  0.0f}, { 0.0f,  0.0f,  1.0f,  1.0f}, {1.0f, 0.0f}},
						{{ 0.5f * x, -0.5f * y, -0.5f * z}, { 1.0f,  0.0f,  0.0f}, { 0.0f,  0.0f,  1.0f,  1.0f}, {0.0f, 1.0f}},
						{{ 0.5f * x, -0.5f * y,  0.5f * z}, { 1.0f,  0.0f,  0.0f}, { 0.0f,  0.0f,  1.0f,  1.0f}, {1.0f, 1.0f}},

						// Top (+Y)
						{{ 0.5f * x,  0.5f * y, -0.5f * z}, { 0.0f,  1.0f,  0.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
						{{-0.5f * x,  0.5f * y, -0.5f * z}, { 0.0f,  1.0f,  0.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
						{{ 0.5f * x,  0.5f * y,  0.5f * z}, { 0.0f,  1.0f,  0.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},
						{{-0.5f * x,  0.5f * y,  0.5f * z}, { 0.0f,  1.0f,  0.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},

						// Bottom (-Y)
						{{ 0.5f * x, -0.5f * y,  0.5f * z}, { 0.0f, -1.0f,  0.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
						{{-0.5f * x, -0.5f * y,  0.5f * z}, { 0.0f, -1.0f,  0.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
						{{ 0.5f * x, -0.5f * y, -0.5f * z}, { 0.0f, -1.0f,  0.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},
						{{-0.5f * x, -0.5f * y, -0.5f * z}, { 0.0f, -1.0f,  0.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
					},
					.indices = {
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
				};

				geometry.hull = createHull<Hull3DType::POLYGON>(geometry.vertices);

				return geometry;
			}

		public:
			using Geometry3DProcess::Geometry3DProcess;
	};
}