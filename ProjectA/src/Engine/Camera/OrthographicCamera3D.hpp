#pragma once

#include "Engine/Camera/Camera3D.hpp"

namespace ProjectA {
	class OrthographicCamera3D : public Camera3D {
		public:
			OrthographicCamera3D(float32 left, float32 right, float32 bottom, float32 top, float32 nearPlane, float32 farPlane);

			void updateProjection(float32 left, float32 right, float32 bottom, float32 top, float32 nearPlane, float32 farPlane);

			// NOTE: Rotation is in radians.
			void updateView(const vec3<float32>& position, float32 rotation);
	};
}