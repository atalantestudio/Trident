#pragma once

#include "pch.hpp"

// TODO: Check order of view and projection members
namespace ProjectA {
	class Camera {
		public:
			mat4x4<float32> projection;
			mat4x4<float32> view;
			mat4x4<float32> projectionView;

		public:
			// TODO: Accept view matrix as 2nd argument?
			Camera(const mat4x4<float32>& projection);

			~Camera() = default;
	};
}