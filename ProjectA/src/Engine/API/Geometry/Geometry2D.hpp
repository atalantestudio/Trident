#pragma once

namespace ProjectA {
	struct Geometry2DDescriptor {
		sequence<uint16> indices;
		sequence<vec2<float32>> vertices;
	};

	class Geometry2D {
		public:
			explicit Geometry2D(const Geometry2DDescriptor& descriptor);

			uint32 getIndexCount() const;

			uint32 getVertexCount() const;

			virtual vec2<float32> computeCenterOfMass() const = 0;

			virtual float32 computeRadius() const = 0;

			// Returns the index of the farthest point of the geometry along d.
			// d does not need to be normalized.
			virtual uint32 support(const vec2<float32>& d) const = 0;

		public:
			sequence<uint16> indices;
			sequence<vec2<float32>> vertices;

			// Average of all vertices.
			vec2<float32> centerOfMass;

			// Distance between the center of mass and the farthest vertex.
			float32 radius;
	};
}