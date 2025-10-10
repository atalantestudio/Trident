#pragma once

namespace ProjectA {
	struct Geometry3DDescriptor {
		sequence<float32> vertexPositionComponents;
		sequence<uint32> vertexPositionIndices;
		sequence<float32> vertexNormalComponents;
		sequence<uint32> vertexNormalIndices;
		sequence<float32> vertexTextureCoordinateComponents;
		sequence<uint32> vertexTextureCoordinateIndices;
	};

	class Geometry3D {
		public:
			explicit Geometry3D() = default;

			explicit Geometry3D(const Geometry3DDescriptor& descriptor) :
				vertexPositionComponents(descriptor.vertexPositionComponents),
				vertexPositionIndices(descriptor.vertexPositionIndices),
				vertexNormalComponents(descriptor.vertexNormalComponents),
				vertexNormalIndices(descriptor.vertexNormalIndices),
				vertexTextureCoordinateComponents(descriptor.vertexTextureCoordinateComponents),
				vertexTextureCoordinateIndices(descriptor.vertexTextureCoordinateIndices)
			{}

			explicit Geometry3D(const ref<GeometryAsset>& geometryAsset) :
				vertexPositionComponents(geometryAsset->getVertexPositionComponents()),
				vertexPositionIndices(geometryAsset->getIndices().count)
			{
				// TODO: Copy the array instead of using a loop.
				for (uint32 indexOffset = 0; indexOffset < vertexPositionIndices.count; indexOffset += 1) {
					vertexPositionIndices[indexOffset] = geometryAsset->getIndices()[indexOffset].positionIndex;
				}
			}

			const sequence<float32>& getVertexPositionComponents() const {
				return vertexPositionComponents;
			}

			const sequence<uint32>& getVertexPositionIndices() const {
				return vertexPositionIndices;
			}

			const sequence<float32>& getVertexNormalComponents() const {
				return vertexNormalComponents;
			}

			const sequence<uint32>& getVertexNormalIndices() const {
				return vertexNormalIndices;
			}

			const sequence<float32>& getVertexTextureCoordinateComponents() const {
				return vertexTextureCoordinateComponents;
			}

			const sequence<uint32>& getVertexTextureCoordinateIndices() const {
				return vertexTextureCoordinateIndices;
			}

			uint32 getTriangleCount() const {
				return static_cast<uint32>(vertexPositionIndices.count) / 3;
			}

		protected:
			sequence<float32> vertexPositionComponents;
			sequence<uint32> vertexPositionIndices;
			sequence<float32> vertexNormalComponents;
			sequence<uint32> vertexNormalIndices;
			sequence<float32> vertexTextureCoordinateComponents;
			sequence<uint32> vertexTextureCoordinateIndices;

			// TODO: Store cluster count?
	};
}