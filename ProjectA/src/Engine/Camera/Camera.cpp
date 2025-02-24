#include "pch.hpp"

#include "Engine/Camera/Camera.hpp"

namespace ProjectA {
	Camera::Camera(const mat4x4<float32>& projection) : projection(projection), view(mat4x4<float32>::identity()), projectionView(projection) {}
}