#pragma once

#include "Engine/API/Physics/Body/Body.hpp"

namespace ProjectA {
	struct RigidBody2D : public Body2D {
		vec2<float32> position = {0.0f, 0.0f};
		vec2<float32> linearVelocity = {0.0_m_s, 0.0_m_s};
		vec2<float32> linearAcceleration = {0.0_m_s2, 0.0_m_s2};

		// Sum of the forces acting on the object's linear properties.
		vec2<float32> linearForces = {0.0_m_s2, 0.0_m_s2};

		float32 rotation = 0.0f;
		float32 angularVelocity = 0.0_m_s;
		float32 angularAcceleration = 0.0_m_s2; // (torque)

		float32 angularForces = 0.0_m_s2;

		vec2<float32> scale = {1.0f, 1.0f};

		// TODO: Scaling forces?

		float32 inverseMass = 1.0f / 1.0_kg;

		// TODO: Inertia/inverse inertia?

		void applyLinearForce(const vec2<float32>& force) override {
			linearForces += force;
		}
	};
}