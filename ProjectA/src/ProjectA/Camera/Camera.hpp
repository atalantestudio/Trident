#pragma once

// TODO: Check order of view and projection members
namespace ProjectA {
	class Camera {
		protected:
			// Row-major ordering
			mat4x4<float> view;

			// Row-major ordering
			mat4x4<float> projection;

			// Row-major ordering
			mat4x4<float> viewProjection;

		public:
			// TODO: Accept view matrix as 2nd argument?
			Camera(const mat4x4<float>& projection);

			~Camera() = default;

			inline const mat4x4<float>& getView() const {
				return view;
			}

			inline const mat4x4<float>& getProjection() const {
				return projection;
			}

			inline const mat4x4<float>& getViewProjection() const {
				return viewProjection;
			}
	};
}