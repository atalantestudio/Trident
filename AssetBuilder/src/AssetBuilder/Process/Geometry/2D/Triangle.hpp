// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Geometry/Geometry2DProcess.hpp"

namespace Trident {
	class TriangleGeometry2D final : public Geometry2DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "2d_triangle";
			}

			BuildGeometry2D createGeometry() const final override {
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
					.flags = GeometryAssetFlags::HAS_HULL,
					.vertexComponentBuffer = vertexComponentBuffer,
					.indexBuffer = indexBuffer,
					.geometry = geometry,
					.hull = hull,
				};
			}

		public:
			using Geometry2DProcess::Geometry2DProcess;
	};
}