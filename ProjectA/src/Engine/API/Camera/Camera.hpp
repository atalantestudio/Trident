#pragma once

namespace ProjectA {
	class Camera {
		public:
			const mat4x4<float32>& getProjection() const {
				return projection;
			}

			const mat4x4<float32>& getView() const {
				return view;
			}

			const mat4x4<float32>& getProjectionView() const {
				return projectionView;
			}

		protected:
			mat4x4<float32> projection = mat4x4<float32>::identity();
			mat4x4<float32> view = mat4x4<float32>::identity();
			mat4x4<float32> projectionView = mat4x4<float32>::identity();
	};
}