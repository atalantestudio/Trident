#pragma once

namespace ProjectA {
	class Geometry3D {
		public:
			const sequence<uint16>& getIndices() const {
				return indices;
			}

			const sequence<vec3<float32>>& getVertices() const {
				return vertices;
			}

			uint32 getIndexCount() const {
				return static_cast<uint32>(indices.count);
			}

			uint32 getVertexCount() const {
				return static_cast<uint32>(vertices.count);
			}

		protected:
			sequence<uint16> indices;
			sequence<vec3<float32>> vertices;
	};
}