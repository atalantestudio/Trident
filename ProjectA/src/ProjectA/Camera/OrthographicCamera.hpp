#pragma once

#include "Camera.hpp"
#include "math/vec/vec3.hpp"

namespace ProjectA {
	class OrthographicCamera : public Camera {
		public:
			OrthographicCamera(float left, float right, float bottom, float top, float nearPlane, float farPlane);

			void updateView(const vec3<float>& position, float rotation);
			void updateProjection(float left, float right, float bottom, float top, float nearPlane, float farPlane);
	};
}