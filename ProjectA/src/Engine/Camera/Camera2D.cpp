#include "pch.hpp"

#include "Engine/Camera/Camera2D.hpp"

namespace ProjectA {
	Camera2D::Camera2D(const mat3x3<float32>& projection) :
		projection(projection)
	{}
}