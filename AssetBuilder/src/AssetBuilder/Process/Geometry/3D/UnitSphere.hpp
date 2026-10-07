// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Geometry/Geometry3DProcess.hpp"

namespace Trident {
	class UnitSphereGeometry3D final : public Geometry3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "3d_unit_sphere";
			}

			BuildGeometry3D createGeometry() const final override {
				static constexpr uint32 X_SEGMENTS = 64;
				static constexpr uint32 Y_SEGMENTS = 64;
				static constexpr uint32 s = X_SEGMENTS + 1;

				BuildGeometry3D geometry = {
					.flags = GeometryAssetFlags::IS_3D,
					.vertices = sequence<BuildVertex3D>((X_SEGMENTS + 1) * (X_SEGMENTS + 1)),
					.indices = sequence<uint32>((X_SEGMENTS + 1) * Y_SEGMENTS * 6),
				};

				uint64 vertexIndex = 0;
				uint32 x = 0;

				while (x <= X_SEGMENTS) {
					const float32 xSegment = static_cast<float32>(x) / X_SEGMENTS;

					uint32 y = 0;

					while (y <= Y_SEGMENTS) {
						const float32 ySegment = static_cast<float32>(y) / Y_SEGMENTS;
						const float32 xPos = cos(xSegment * tau<float32>()) * sin(ySegment * pi<float32>());
						const float32 yPos = cos(ySegment * pi<float32>());
						const float32 zPos = sin(xSegment * tau<float32>()) * sin(ySegment * pi<float32>());

						geometry.vertices[vertexIndex] = {
							.position = svec3<float32>(xPos, yPos, zPos) * 0.5f,
							.normal = {xPos, yPos, zPos},
							.textureCoordinate = {xSegment, ySegment},
						};

						vertexIndex += 1;
						y += 1;
					}

					x += 1;
				}

				uint64 indexOffset = 0;
				uint32 y = 0;

				while (y < Y_SEGMENTS) {
					if (y & 1) {
						sint32 x = X_SEGMENTS;

						while (x >= 0) {
							const uint32 i0 = (y + 1) * s + x;
							const uint32 i1 = (y + 0) * s + x;
							const uint32 i2 = (y + 1) * s + (x - 1);
							const uint32 i3 = (y + 0) * s + (x - 1);

							geometry.indices[indexOffset + 0] = i0;
							geometry.indices[indexOffset + 1] = i1;
							geometry.indices[indexOffset + 2] = i2;
							geometry.indices[indexOffset + 3] = i1;
							geometry.indices[indexOffset + 4] = i3;
							geometry.indices[indexOffset + 5] = i2;

							indexOffset += 6;
							x -= 1;
						}
					}
					else {
						uint32 x = 0;

						while (x < X_SEGMENTS) {
							const uint32 i0 = (y + 0) * s + x;
							const uint32 i1 = (y + 1) * s + x;
							const uint32 i2 = (y + 0) * s + (x + 1);
							const uint32 i3 = (y + 1) * s + (x + 1);

							geometry.indices[indexOffset + 0] = i0;
							geometry.indices[indexOffset + 1] = i1;
							geometry.indices[indexOffset + 2] = i2;
							geometry.indices[indexOffset + 3] = i1;
							geometry.indices[indexOffset + 4] = i3;
							geometry.indices[indexOffset + 5] = i2;

							indexOffset += 6;
							x += 1;
						}
					}

					y += 1;
				}

				generateTangents(geometry);

				return geometry;
			}

		public:
			using Geometry3DProcess::Geometry3DProcess;
	};
}