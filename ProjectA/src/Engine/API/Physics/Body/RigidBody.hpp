#pragma once

#include "Engine/API/Physics/Body/Body.hpp"

namespace ProjectA {
	struct RigidBody2D : public Body2D {
		vec2<float32> position = {0.0_m, 0.0_m};
		vec2<float32> linearVelocity = {0.0_m_s, 0.0_m_s};
		vec2<float32> linearAcceleration = {0.0_m_s2, 0.0_m_s2};

		// Sum of the forces acting on the object's linear properties.
		vec2<float32> force = {0.0f, 0.0f};

		// TODO: Use correct SI unit.
		float32 rotation = 0.0f;

		float32 angularVelocity = 0.0_m_s;

		// Torque = inertia * angular acceleration (around the COM).
		float32 torque = 0.0f;

		// NOTE: The scale of an entity isn't meant to be dynamically updated.
		vec2<float32> scale = {1.0f, 1.0f};

		float32 inverseMass = 1.0f / 1.0_kg;

		// TODO: Differentiate static friction from dynamic friction.
		float32 friction = 0.0f;

		// Inertia = mass distribution over the shape
		// TODO: Use correct SI unit.
		float32 inverseInertia = 1.0f;

		void applyForce(const vec2<float32>& force) override {
			this->force += force;
		}

		void applyImpulse(const vec2<float32>& j, const vec2<float32>& r) override {
			linearVelocity += j * inverseMass;
			angularVelocity += parallelogramArea(r, j) * inverseInertia;
		}
	};
}