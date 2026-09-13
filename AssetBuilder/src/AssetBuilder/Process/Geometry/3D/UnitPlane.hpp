// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Geometry/Geometry3DProcess.hpp"

namespace Trident {
	class UnitPlaneGeometry3D final : public Geometry3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_unit_plane";
			}

			BuildGeometry3D createGeometry() const final override {
				const float32 x = 1.0f;
				const float32 z = 1.0f;

				BuildGeometry3D geometry = {
					.flags = GeometryAssetFlags::IS_3D | GeometryAssetFlags::HAS_HULL,
					.vertices = {
						{{-0.5f * x,  0.0f,  0.5f * z}, { 0.0f,  1.0f,  0.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
						{{ 0.5f * x,  0.0f,  0.5f * z}, { 0.0f,  1.0f,  0.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
						{{-0.5f * x,  0.0f, -0.5f * z}, { 0.0f,  1.0f,  0.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},
						{{ 0.5f * x,  0.0f, -0.5f * z}, { 0.0f,  1.0f,  0.0f}, { 1.0f,  0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
					},
					.indices = {
						0, 1, 2,
						1, 3, 2,
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
					.hullLineIndices = {
						0, 1,
						1, 3,
						3, 2,
						2, 0,
					},
				};

				geometry.hull = createHull<Hull3DType::POLYGON>(geometry.vertices);

				return geometry;
			}

		public:
			using Geometry3DProcess::Geometry3DProcess;
	};
}