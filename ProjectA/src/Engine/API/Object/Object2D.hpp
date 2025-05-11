#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"

namespace ProjectA {
	struct Object2DDescriptor {
		uint32 geometryIndex;
		uint32 materialIndex;

		// TODO: Only in debug mode.
		const char* debugName;
	};

	class Object2D {
		public:
			explicit Object2D(const Object2DDescriptor& descriptor);

		public:
			uint32 geometryIndex;
			uint32 materialIndex;

			vec2<float32> position = {0.0f, 0.0f};
			vec2<float32> linearVelocity = {0.0f, 0.0f};
			vec2<float32> linearAcceleration = {0.0f, 0.0f};

			float32 rotation = 0.0f;
			float32 angularVelocity = 0.0f;
			float32 angularAcceleration = 0.0f;

			vec2<float32> scale = {1.0f, 1.0f};

			mat3x3<float32> transform = mat3x3<float32>::identity();

			const char* debugName;
	};
}