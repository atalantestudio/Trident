#include "pch.hpp"

#include "ProjectA/Camera/Camera.hpp"

namespace ProjectA {
	Camera::Camera(const mat4x4<float>& projection) :
		// TODO: Restore mat4x4<float>::identity()
		view(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		),
		projection(projection),
		viewProjection(projection) {}
}