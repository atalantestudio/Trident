#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"

namespace ProjectA {
	struct Object2DDescriptor {
		uint32 geometryIndex;
		uint32 materialIndex;

		// TODO: Only in debug mode
		const char* debugName;
	};

	class Object2D {
		public:
			Object2D(const Object2DDescriptor& descriptor);

		public:
			const uint32 geometryIndex;
			const uint32 materialIndex;
			mat3x3<float32> transform;
			vec2<float32> position;
	};
}