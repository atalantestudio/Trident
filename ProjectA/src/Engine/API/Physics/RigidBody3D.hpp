#pragma once

#include "Engine/API/Geometry/3D/Geometry3D.hpp"

namespace ProjectA {
	class RigidBody3D {
		public:
			void updateTransform() {
				transform =
					// TODO(3D): COM
					// mat4x4<float32>::translation(-centerOfMass) *
					mat4x4<float32>::scaling(scale) *
					mat4x4<float32>::rotation({1.0f, 0.0f, 0.0f}, -rotation[0]) *
					mat4x4<float32>::rotation({0.0f, 1.0f, 0.0f}, -rotation[1]) *
					mat4x4<float32>::rotation({0.0f, 0.0f, 1.0f}, -rotation[2]) *
					mat4x4<float32>::translation(position);
			}

		public:
			mat4x4<float32> transform = mat4x4<float32>::identity();

			// Position of the center of mass, in world-space.
			vec3<float32> position = {0.0_m, 0.0_m, 0.0_m};

			vec3<float32> rotation = {0.0_rad, 0.0_rad, 0.0_rad};

			// NOTE: The scale of an entity isn't meant to be dynamically updated.
			vec3<float32> scale = {1.0f, 1.0f, 1.0f};

			uint32 geometryIndex;
	};
}