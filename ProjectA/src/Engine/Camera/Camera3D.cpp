#include "pch.hpp"

#include "Engine/Camera/Camera3D.hpp"

namespace ProjectA {
	Camera3D::Camera3D(const mat4x4<float32>& projection) :
		projection(projection),
		view(mat4x4<float32>::identity()),
		projectionView(projection)
	{}
}