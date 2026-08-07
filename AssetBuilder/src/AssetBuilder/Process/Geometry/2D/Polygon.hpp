// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Utilities/Utilities.hpp"
#include "AssetBuilder/Process/Geometry/Geometry2DProcess.hpp"

namespace Trident {
	class PolygonGeometry2D final : public Geometry2DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "2d_polygon";
			}

			BuildGeometry2D createGeometry() const final override {
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
					.flags = GeometryAssetFlags::HAS_HULL,
					.vertexComponentBuffer = vertexComponentBuffer,
					.indexBuffer = indexBuffer,
					.geometry = geometry,
					.hull = hull,
				};
			}

		public:
			explicit PolygonGeometry2D(AssetBuilderLibrary* assetLibrary) :
				Geometry2DProcess(assetLibrary)
			{}
	};
}