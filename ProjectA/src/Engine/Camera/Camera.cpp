#include "pch.hpp"

#include "Engine/Camera/Camera.hpp"

namespace ProjectA {
	Camera::Camera(const lyah::mat4x4<float32>& projection) : view(lyah::mat4x4<float32>::identity()), projection(projection), viewProjection(projection) {}
}