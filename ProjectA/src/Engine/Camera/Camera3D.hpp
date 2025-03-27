#pragma once

// TODO: Remove this include.
#include "pch.hpp"

namespace ProjectA {
	class Camera3D {
		public:
			Camera3D(const mat4x4<float32>& projection);

		public:
			mat4x4<float32> projection;
			mat4x4<float32> view;
			mat4x4<float32> projectionView;
	};
}