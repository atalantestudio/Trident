// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Geometry/Geometry2DProcess.hpp"

namespace Trident {
	class UnitCircleGeometry2D final : public Geometry2DProcess {
		private:
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

		protected:
			view<char8> getAssetName() const final override {
				return "2d_unit_circle";
			}

			BuildGeometry2D createGeometry() const final override {
				std::vector<float32> vertexComponentBuffer;
				std::vector<uint32> indexBuffer;

				const Geometry2D geometry = createUnitCircleGeometry(32, vertexComponentBuffer, indexBuffer);
				const Hull2D hull = createHull<Hull2DType::CIRCLE>(geometry, vertexComponentBuffer, indexBuffer);

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