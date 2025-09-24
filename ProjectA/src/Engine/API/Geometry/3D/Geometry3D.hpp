#pragma once

namespace ProjectA {
	class Geometry3D {
		public:
			const sequence<vec3<float32>>& getVertexPositions() const {
				return vertexPositions;
			}

			const sequence<uint32>& getVertexPositionIndices() const {
				return vertexPositionIndices;
			}

			const sequence<vec3<float32>>& getVertexNormals() const {
				return vertexNormals;
			}

			const sequence<uint32>& getVertexNormalIndices() const {
				return vertexNormalIndices;
			}

			const sequence<vec2<float32>>& getVertexTextureCoordinates() const {
				return vertexTextureCoordinates;
			}

			const sequence<uint32>& getVertexTextureCoordinateIndices() const {
				return vertexTextureCoordinateIndices;
			}

			uint32 getTriangleCount() const {
				return static_cast<uint32>(vertexPositionIndices.count) / 3;
			}

		protected:
			sequence<vec3<float32>> vertexPositions;
			sequence<uint32> vertexPositionIndices;
			sequence<vec3<float32>> vertexNormals;
			sequence<uint32> vertexNormalIndices;
			sequence<vec2<float32>> vertexTextureCoordinates;
			sequence<uint32> vertexTextureCoordinateIndices;
	};
}