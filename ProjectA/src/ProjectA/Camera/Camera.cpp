#include "pch.hpp"

#include "ProjectA/Camera/Camera.hpp"

namespace ProjectA {
	Camera::Camera(const mat4x4<float>& projection) : view(mat4x4<float>::identity()), projection(projection), viewProjection(projection) {}
}