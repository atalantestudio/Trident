#pragma once

namespace ProjectA {
	struct Geometry2DDescriptor {
		chain<svec2<float32>> vertices;
	};

	class Geometry2D {
		public:
			explicit Geometry2D(const Geometry2DDescriptor& descriptor);

			const chain<svec2<float32>>& getVertices() const;

			const svec2<float32>& getCenterOfMass() const;

			float32 getRadius() const;

		private:
			chain<svec2<float32>> vertices;

			svec2<float32> centerOfMass;

			// Distance between the center of mass and the farthest vertex.
			float32 radius = 0.0f;
	};
}