// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/GeometryAsset.hpp"

namespace Trident {
	struct Geometry3DDescriptor {
		sequence<float32> vertexPositionComponents;
		sequence<uint32> vertexPositionIndices;
		sequence<float32> vertexNormalComponents;
		sequence<uint32> vertexNormalIndices;
		sequence<float32> vertexTextureCoordinateComponents;
		sequence<uint32> vertexTextureCoordinateIndices;
		sequence<float32> vertexTangentComponents;
	};

	// TODO: Unify the vertex position/normal/texture coordinate indices since they must be the same length.
	class Geometry3D {
		public:
			static constexpr uint32 VERTEX_POSITION_STRIDE = 3;
			static constexpr uint32 VERTEX_NORMAL_STRIDE = 3;
			static constexpr uint32 VERTEX_TEXTURE_COORDINATE_STRIDE = 2;

		public:
			explicit Geometry3D() = default;

			explicit Geometry3D(const Geometry3DDescriptor& descriptor) :
				vertexPositionComponents(descriptor.vertexPositionComponents),
				vertexNormalComponents(descriptor.vertexNormalComponents),
				vertexTextureCoordinateComponents(descriptor.vertexTextureCoordinateComponents),
				vertexTangentComponents(descriptor.vertexPositionIndices.count()),
				vertexPositionIndices(descriptor.vertexPositionIndices),
				vertexNormalIndices(descriptor.vertexNormalIndices),
				vertexTextureCoordinateIndices(descriptor.vertexTextureCoordinateIndices)
			{
				calculateVertexTangentComponents();
			}

			explicit Geometry3D(ref<GeometryAsset> geometryAsset) :
				vertexPositionComponents(geometryAsset->getVertexPositionComponents()),
				vertexNormalComponents(geometryAsset->getVertexNormalComponents()),
				vertexTextureCoordinateComponents(geometryAsset->getVertexTextureCoordinateComponents()),
				vertexTangentComponents(geometryAsset->getVertexPositionIndices().count()),
				vertexPositionIndices(geometryAsset->getVertexPositionIndices()),
				vertexNormalIndices(geometryAsset->getVertexNormalIndices()),
				vertexTextureCoordinateIndices(geometryAsset->getVertexTextureCoordinateIndices())
			{
				calculateVertexTangentComponents();
			}

			const sequence<float32>& getVertexPositionComponents() const {
				return vertexPositionComponents;
			}

			const sequence<float32>& getVertexNormalComponents() const {
				return vertexNormalComponents;
			}

			const sequence<float32>& getVertexTextureCoordinateComponents() const {
				return vertexTextureCoordinateComponents;
			}

			const sequence<uint32>& getVertexPositionIndices() const {
				return vertexPositionIndices;
			}

			const sequence<uint32>& getVertexNormalIndices() const {
				return vertexNormalIndices;
			}

			const sequence<uint32>& getVertexTextureCoordinateIndices() const {
				return vertexTextureCoordinateIndices;
			}

			const sequence<float32>& getVertexTangentComponents() const {
				return vertexTangentComponents;
			}

			uint32 getTriangleCount() const {
				return static_cast<uint32>(vertexPositionIndices.count()) / 3;
			}

		private:
			void calculateVertexTangentComponents() {
				uint64 triangleIndex = 0;

				while (triangleIndex < getTriangleCount()) {
					const uint32 index_pA = vertexPositionIndices[triangleIndex * 3 + 0];
					const uint32 index_pB = vertexPositionIndices[triangleIndex * 3 + 1];
					const uint32 index_pC = vertexPositionIndices[triangleIndex * 3 + 2];
					const uint32 index_uvA = vertexTextureCoordinateIndices[triangleIndex * 3 + 0];
					const uint32 index_uvB = vertexTextureCoordinateIndices[triangleIndex * 3 + 1];
					const uint32 index_uvC = vertexTextureCoordinateIndices[triangleIndex * 3 + 2];

					const vec3<float32> pA = {
						vertexPositionComponents[index_pA * 3 + 0],
						vertexPositionComponents[index_pA * 3 + 1],
						vertexPositionComponents[index_pA * 3 + 2],
					};
					const vec3<float32> pB = {
						vertexPositionComponents[index_pB * 3 + 0],
						vertexPositionComponents[index_pB * 3 + 1],
						vertexPositionComponents[index_pB * 3 + 2],
					};
					const vec3<float32> pC = {
						vertexPositionComponents[index_pC * 3 + 0],
						vertexPositionComponents[index_pC * 3 + 1],
						vertexPositionComponents[index_pC * 3 + 2],
					};
					const vec2<float32> uvA = {
						vertexTextureCoordinateComponents[index_uvA * 2 + 0],
						vertexTextureCoordinateComponents[index_uvA * 2 + 1],
					};
					const vec2<float32> uvB = {
						vertexTextureCoordinateComponents[index_uvB * 2 + 0],
						vertexTextureCoordinateComponents[index_uvB * 2 + 1],
					};
					const vec2<float32> uvC = {
						vertexTextureCoordinateComponents[index_uvC * 2 + 0],
						vertexTextureCoordinateComponents[index_uvC * 2 + 1],
					};

					const vec3<float32> delta_pB = pB - pA;
					const vec3<float32> delta_pC = pC - pA;
					const vec2<float32> delta_uvB = uvB - uvA;
					const vec2<float32> delta_uvC = uvC - uvA;
					const float32 f = 1.0f / (delta_uvB[0] * delta_uvC[1] - delta_uvC[0] * delta_uvB[1]);
					const vec3<float32> tangent = f * (delta_uvC[1] * delta_pB - delta_uvB[1] * delta_pC);

					vertexTangentComponents[triangleIndex * 3 + 0] = tangent[0];
					vertexTangentComponents[triangleIndex * 3 + 1] = tangent[1];
					vertexTangentComponents[triangleIndex * 3 + 2] = tangent[2];

					triangleIndex += 1;
				}
			}

		protected:
			sequence<float32> vertexPositionComponents;
			sequence<float32> vertexNormalComponents;
			sequence<float32> vertexTextureCoordinateComponents;
			sequence<float32> vertexTangentComponents;
			sequence<uint32> vertexPositionIndices;
			sequence<uint32> vertexNormalIndices;
			sequence<uint32> vertexTextureCoordinateIndices;
	};
}