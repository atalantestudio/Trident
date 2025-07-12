#pragma once

namespace ProjectA {
	class Geometry2D {
		public:
			uint32 getIndexCount() const {
				return static_cast<uint32>(indices.count);
			}

			uint32 getVertexCount() const {
				return static_cast<uint32>(vertices.count);
			}

			virtual vec2<float32> calculateCenterOfMass() const = 0;

			virtual float32 calculateMass(float32 density) const = 0;

			virtual float32 calculateRadius() const = 0;

			// Returns the farthest point on the geometry along `d`. `d` does not need to be normalized.
			virtual vec2<float32> support(const vec2<float32>& d) const = 0;

			// Returns the index of the farthest point on the geometry along `d`. `d` does not need to be normalized.
			// NOTE: Some geometries like CircleGeometry2D return more precise support points.
			//       In one of these cases, `support` must be used.
			virtual uint32 supportIndex(const vec2<float32>& d) const = 0;

		public:
			// The index sequence must be readable with both TRIANGLE_LIST and LINE_STRIP topologies.
			sequence<uint32> indices;

			sequence<vec2<float32>> vertices;

			// Average of all vertices.
			vec2<float32> centerOfMass;

			// Distance between the center of mass and the farthest vertex.
			float32 radius;
	};
}