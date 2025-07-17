#pragma once

#include "Engine/API/Physics/ContactManifold.hpp"
#include "Engine/API/Scene/Scene2D.hpp"
#include "Engine/ECS/Entity.hpp"

namespace ProjectA {
	struct Arbiter2D {
		Scene2D* scene;
		PhysicalObjectCopy2D A;
		PhysicalObjectCopy2D B;
		ContactManifold2D contactManifold;

		void update(const ContactManifold2D& manifold) {
			contactManifold = manifold;

			// TODO
		}

		void precompute(float32 inverseDeltaTime) {
			static constexpr float32 ALLOWED_PENETRATION = 0.01f;
			static constexpr float32 PENETRATION_BIAS = true ? 0.2f : 0.0f;

			const ref<RigidBody2D>& rigidBodyA = std::static_pointer_cast<RigidBody2D>(scene->getBody(A.entity));
			const ref<RigidBody2D>& rigidBodyB = std::static_pointer_cast<RigidBody2D>(scene->getBody(B.entity));

			for (uint8 contactPointIndex = 0; contactPointIndex < contactManifold.contactCount; contactPointIndex += 1) {
				Contact2D& contact = contactManifold.contacts[contactPointIndex];

				// Distance from the contact to the shape's COM.
				contact.rA = contact.position - rigidBodyA->position;
				contact.rB = contact.position - rigidBodyB->position;

				const vec2<float32> relativeVelocity =
					rigidBodyB->linearVelocity + cross(rigidBodyB->angularVelocity, contact.rB) -
					rigidBodyA->linearVelocity - cross(rigidBodyA->angularVelocity, contact.rA);

				// Precompute normal mass, tangent mass, and bias.
				const float32 rn1 = dot(contact.rA, contactManifold.contactNormal);
				const float32 rn2 = dot(contact.rB, contactManifold.contactNormal);

				float32 normal =
					(dot(contact.rA, contact.rA) - rn1 * rn1) * rigidBodyA->inverseInertia +
					(dot(contact.rB, contact.rB) - rn2 * rn2) * rigidBodyB->inverseInertia +
					rigidBodyA->inverseMass + rigidBodyB->inverseMass;

				/* const float32 inverseMassSum =
					lyah::sqrt(parallelogramArea(rA, contactManifold.contactNormal)) * rigidBodyA->inverseInertia +
					lyah::sqrt(parallelogramArea(rB, contactManifold.contactNormal)) * rigidBodyB->inverseInertia +
					rigidBodyA->inverseMass + rigidBodyB->inverseMass; */

				contact.massNormal = 1.0f / normal;

				/* const vec2<float32> tangent = cross(contactManifold.contactNormal, 1.0f);

				const float32 rt1 = dot(rA, tangent);
				const float32 rt2 = dot(rB, tangent);
				const float32 tangentN = (rigidBodyA->inverseInertia * (dot(rA, rA) - rt1 * rt1) + rigidBodyB->inverseInertia * (dot(rB, rB) - rt2 * rt2)) + rigidBodyA->inverseMass + rigidBodyB->inverseMass;

				contact.massTangent = 1.0f / tangentN; */

				contact.bias = -PENETRATION_BIAS * (inverseDeltaTime/*  / 10.0f */) * std::min(0.0f, contactManifold.penetration + ALLOWED_PENETRATION);
				// contact.accumulatedNormalImpulseBias = 0.0f;

				/* // Accumulate impulses.
				{
					// Apply normal + friction impulse.
					const vec2<float32> j = contact.accumulatedNormalImpulse * contactManifold.contactNormal + contact.accumulatedTangentImpulse * tangent;

					rigidBodyA->applyImpulse(-j, rA);
					rigidBodyB->applyImpulse(j, rB);
				} */
			}
		}

		void applyImpulse() {
			const ref<RigidBody2D>& rigidBodyA = std::static_pointer_cast<RigidBody2D>(scene->getBody(A.entity));
			const ref<RigidBody2D>& rigidBodyB = std::static_pointer_cast<RigidBody2D>(scene->getBody(B.entity));

			for (uint8 contactPointIndex = 0; contactPointIndex < contactManifold.contactCount; contactPointIndex += 1) {
				Contact2D& contact = contactManifold.contacts[contactPointIndex];

				const vec2<float32> relativeVelocity =
					rigidBodyB->linearVelocity + cross(rigidBodyB->angularVelocity, contact.rB) -
					rigidBodyA->linearVelocity - cross(rigidBodyA->angularVelocity, contact.rA);

				// TODO
				const float32 vn = dot(relativeVelocity, contactManifold.contactNormal);

				// NOTE: Restitution gets applied here (0.0f).
				float32 dPn = contact.massNormal * (-vn + contact.bias);

				// Clamp the accumulated impulse.
				/* {
					const float32 previousANI = contact.accumulatedNormalImpulse;

					contact.accumulatedNormalImpulse = std::max(0.0f, previousANI + dPn);

					dPn = contact.accumulatedNormalImpulse - previousANI;
				} */
				dPn = std::max(0.0f, dPn);

				const vec2<float32> normalImpulse = contactManifold.contactNormal * dPn;

				rigidBodyA->applyImpulse(-normalImpulse, contact.rA);
				rigidBodyB->applyImpulse(normalImpulse, contact.rB);

				// TODO: Friction impulse.
				// TODO: Angular normal/friction impulses.

				// ENGINE_DEBUG("imp []", (int) contactPointIndex);
			}
		}
	};
}