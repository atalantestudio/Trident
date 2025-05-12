#pragma once

namespace ProjectA {
	struct Geometry2DDescriptor {
		chain<vec2<float32>> vertices;
		chain<uint16> indices;
	};

	class Geometry2D {
		public:
			explicit Geometry2D(const Geometry2DDescriptor& descriptor);

			// Returns the farthest point of the geometry in the direction d.
			const vec2<float32>& support(const vec2<float32>& d) const;

		public:
			chain<vec2<float32>> vertices;
			chain<uint16> indices;
			vec2<float32> centerOfMass;

			// Distance between the center of mass and the farthest vertex.
			float32 radius = 0.0f;
	};
}