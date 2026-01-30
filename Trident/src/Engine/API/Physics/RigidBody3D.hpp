// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/3D/Geometry3D.hpp"
#include "Engine/API/Physics/Axis.hpp"

namespace Trident {
	class RigidBody3D {
		public:
			void updateTransform() {
				transform =
					mat4x4<float32>::scaling(scale) *
					mat4x4<float32>::rotation(RIGHT, -rotation[0]) *
					mat4x4<float32>::rotation(UP, -rotation[1]) *
					mat4x4<float32>::rotation(FORWARD, -rotation[2]) *
					mat4x4<float32>::translation(position);
			}

		public:
			mat4x4<float32> transform = mat4x4<float32>::identity();

			// Position of the center of mass in world-space.
			vec3<float32> position = {0.0_m, 0.0_m, 0.0_m};

			vec3<float32> rotation = {0.0_rad, 0.0_rad, 0.0_rad};

			// NOTE: The scale of an entity isn't meant to be dynamically updated.
			vec3<float32> scale = {1.0f, 1.0f, 1.0f};

			uint32 geometryIndex;
			uint32 materialIndex;
	};
}