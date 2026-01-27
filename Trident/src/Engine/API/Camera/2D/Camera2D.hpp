#pragma once

namespace Trident {
	class Camera2D {
		public:
			explicit Camera2D(const mat3x3<float32>& projection) :
				projection(projection)
			{}

		public:
			mat3x3<float32> projection;
	};
}