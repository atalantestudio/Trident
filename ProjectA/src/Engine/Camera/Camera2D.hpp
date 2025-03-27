#pragma once

namespace ProjectA {
	class Camera2D {
		public:
			Camera2D(const mat3x3<float32>& projection);

		public:
			mat3x3<float32> projection;
	};
}