#pragma once

#include "Engine/API/Object/PhysicalObject.hpp"
#include "Engine/API/Physics/ContactManifold.hpp"
#include "Engine/ECS/Entity.hpp"

namespace ProjectA {
	struct Arbiter2D {
		PhysicslObjectCopy2D A;
		PhysicslObjectCopy2D B;
		ContactManifold2D contactManifold;

		void update(const ContactManifold2D& manifold) {
			contactManifold = manifold;

			// TODO
		}

		void calculateImpulse() {
			static constexpr float32 PENETRATION_BIAS = 0.01f;
			static constexpr float32 POSITION_BIAS = 0.2f; // 0.0f

			const ref<RigidBody2D>& bodyA = std::static_pointer_cast<RigidBody2D>(scene->getBody(A));
			const ref<RigidBody2D>& bodyB = std::static_pointer_cast<RigidBody2D>(scene->getBody(B));

			for (ContactPoint2D& contactPoint : contactManifold.contactPoints) {
				// Body position to the contact point.
				const vec2<float32> rA = contactPoint.position - bodyA->position;
				const vec2<float32> rB = contactPoint.position - bodyB->position;

				// Precompute normal mass, tangent mass, and bias.
				const float32 rn1 = dot(rA, contactPoint.normal);
				const float32 rn2 = dot(rB, contactPoint.normal);
				const float32 normal = (bodyA->inverseInertia * (dot(rA, rA) - rn1 * rn1) + bodyB->inverseInertia * (dot(rB, rB) - rn2 * rn2)) + bodyA->inverseMass + bodyB->inverseMass;

				contactPoint.massNormal = 1.0f / normal;

				const vec2<float32> tangent = cross(contactPoint.normal, 1.0f);

				const float32 rt1 = dot(rA, tangent);
				const float32 rt2 = dot(rB, tangent);
				const float32 tangentN = (bodyA->inverseInertia * (dot(rA, rA) - rt1 * rt1) + bodyB->inverseInertia * (dot(rB, rB) - rt2 * rt2)) + bodyA->inverseMass + bodyB->inverseMass;

				contactPoint.massTangent = 1.0f / tangentN;

				// contactPoint.bias = -POSITION_BIAS * inv_dt * std::min(0.0f, contactPoint.separation + PENETRATION_BIAS);
				contactPoint.bias = 0.0f;

				// Accumulate impulses.
				{
					// Apply normal + friction impulse
					const vec2<float32> P =
						contactPoint.accumulatedNormalImpulse * contactPoint.normal +
						contactPoint.accumulatedTangentImpulse * tangent;

					bodyA->linearVelocity -= P * bodyA->inverseMass;
					bodyA->angularVelocity -= area(rA, P) * bodyA->inverseInertia;

					bodyB->linearVelocity += P * bodyB->inverseMass;
					bodyB->angularVelocity += area(rB, P) * bodyB->inverseInertia;
				}
			}
		}

		void applyImpulse() {
			const ref<RigidBody2D>& bodyA = std::static_pointer_cast<RigidBody2D>(scene->getBody(A));
			const ref<RigidBody2D>& bodyB = std::static_pointer_cast<RigidBody2D>(scene->getBody(B));

			for (ContactPoint2D& contactPoint : contactManifold.contactPoints) {
				contactPoint.rA = contactPoint.position - bodyA->position;
				contactPoint.rB = contactPoint.position - bodyB->position;

				// Calculate the relative velocity at the contact point.
				const vec2<float32> relativeVelocity =
					bodyB->linearVelocity + cross(bodyB->angularVelocity, contactPoint.rB) -
					bodyA->linearVelocity - cross(bodyA->angularVelocity, contactPoint.rA);

				// TODO
			}
		}
	};
}