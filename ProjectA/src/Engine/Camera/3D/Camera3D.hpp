#pragma once

namespace ProjectA {
	class Camera3D {
		public:
			explicit Camera3D(const mat4x4<float32>& projection) :
				projection(projection),
				view(mat4x4<float32>::identity()),
				projectionView(projection)
			{}

		public:
			mat4x4<float32> projection;
			mat4x4<float32> view;
			mat4x4<float32> projectionView;
	};
}