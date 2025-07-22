#pragma once

#include "Engine/API/Physics/Contact/ContactManifold.hpp"
#include "Engine/API/Scene/Scene2D.hpp"
#include "Engine/ECS/Entity.hpp"

namespace ProjectA {
	class Arbiter2D {
		public:
			void update(const ContactManifold2D& manifold) {
				contactManifold = manifold;

				// TODO
			}

			void precompute(float32 inverseDeltaTime) {
				static constexpr float32 ALLOWED_PENETRATION = 0.01f;
				static constexpr float32 PENETRATION_BIAS = true ? 0.2f : 0.0f;

				const ref<RigidBody2D>& rigidBodyA = std::static_pointer_cast<RigidBody2D>(scene->getBody(A.entity));
				const ref<RigidBody2D>& rigidBodyB = std::static_pointer_cast<RigidBody2D>(scene->getBody(B.entity));

				// contactManifold.tangent = perpendicular(contactManifold.normal, 1.0f);
				// combinedFriction = lyah::sqrt(rigidBodyA->friction * rigidBodyB->friction);

				for (uint8 contactPointIndex = 0; contactPointIndex < contactManifold.getContactCount(); contactPointIndex += 1) {
					Contact2D& contact = contactManifold.contacts[contactPointIndex];

					// Distance from the contact to the shape's COM.
					contact.rA = contact.position - rigidBodyA->position;
					contact.rB = contact.position - rigidBodyB->position;

					// Precompute normal mass, tangent mass, and bias.
					const float32 rnA = dot(contact.rA, contactManifold.normal);
					const float32 rnB = dot(contact.rB, contactManifold.normal);

					const float32 massNormal =
						(dot(contact.rA, contact.rA) - rnA * rnA) * rigidBodyA->inverseInertia +
						(dot(contact.rB, contact.rB) - rnB * rnB) * rigidBodyB->inverseInertia +
						rigidBodyA->inverseMass +
						rigidBodyB->inverseMass;

					contact.massNormal = 1.0f / massNormal;

					/* const float32 rtA = dot(contact.rA, contactManifold.tangent);
					const float32 rtB = dot(contact.rB, contactManifold.tangent);
					const float32 massTangent =
						(dot(contact.rA, contact.rA) - rtA * rtA) * rigidBodyA->inverseInertia +
						(dot(contact.rB, contact.rB) - rtB * rtB) * rigidBodyB->inverseInertia +
						rigidBodyA->inverseMass +
						rigidBodyB->inverseMass;

					contact.massTangent = 1.0f / massTangent; */

					contact.bias = -PENETRATION_BIAS * inverseDeltaTime * std::min(0.0f, contact.separation + ALLOWED_PENETRATION);

					/* // Accumulate impulses.
					{
						// Apply normal + friction impulse.
						const vec2<float32> j = contact.accumulatedNormalImpulse * contactManifold.normal + contact.accumulatedTangentImpulse * tangent;

						rigidBodyA->applyImpulse(-j, rA);
						rigidBodyB->applyImpulse(j, rB);
					} */
				}
			}

			void applyImpulse() {
				const ref<RigidBody2D>& rigidBodyA = std::static_pointer_cast<RigidBody2D>(scene->getBody(A.entity));
				const ref<RigidBody2D>& rigidBodyB = std::static_pointer_cast<RigidBody2D>(scene->getBody(B.entity));

				for (uint8 contactPointIndex = 0; contactPointIndex < contactManifold.getContactCount(); contactPointIndex += 1) {
					Contact2D& contact = contactManifold.contacts[contactPointIndex];

					// Box2D Lite:
					// contact.position = (-0.500000000, 0.00000000)
					// contact.separation = -0.0416660905
					// contact.rA = (-0.500000000, 10.0000000)
					// contact.rB = (-0.500000000, -0.458333910)
					// contact.bias = 0.379993081
					// contact.massNnormal = 80.0000076
					// dv = (0.00000000, -8.49999714)

					// Custom:
					// contact.position = (0.500000000, 0.00000000)
					// contact.separation = -0.0416660905
					// contact.rA = (0.500000000, 10.0000000)
					// contact.rB = (0.500000000, -0.458333910)
					// contact.bias = 0.379993081
					// contact.massNormal = 50.000000
					// dv = (0.00000000, -8.49999714)

					const vec2<float32> dv =
						rigidBodyB->linearVelocity + cross(rigidBodyB->angularVelocity, contact.rB) -
						rigidBodyA->linearVelocity - cross(rigidBodyA->angularVelocity, contact.rA);

					const float32 vn = dot(dv, contactManifold.normal);

					// NOTE: Restitution gets applied here (0.0f).
					float32 normalImpulseLength = contact.massNormal * (-vn + contact.bias);

					// Clamp the accumulated impulse.
					/* {
						const float32 previousANI = contact.accumulatedNormalImpulse;

						contact.accumulatedNormalImpulse = std::max(0.0f, previousANI + normalImpulseLength);

						normalImpulseLength = contact.accumulatedNormalImpulse - previousANI;
					} */
					normalImpulseLength = std::max(0.0f, normalImpulseLength);

					const vec2<float32> normalImpulse = contactManifold.normal * normalImpulseLength;

					// Apply normal impulse.
					rigidBodyA->applyImpulse(-normalImpulse, contact.rA);
					rigidBodyB->applyImpulse(normalImpulse, contact.rB);

					// TODO: Friction impulse.
					/* const float32 vt = dot(dv, contactManifold.tangent);
					float32 frictionImpulseLength = contact.massTangent * -vt;

					const float32 maxPt = combinedFriction * normalImpulseLength;

					// TODO: lyah::clamp
					frictionImpulseLength = std::clamp(frictionImpulseLength, -maxPt, maxPt);

					const vec2<float32> tangentImpulse = contactManifold.tangent * frictionImpulseLength;

					// Apply tangent impulse.
					rigidBodyA->applyImpulse(-tangentImpulse, contact.rA);
					rigidBodyB->applyImpulse(tangentImpulse, contact.rB); */
				}
			}

		public:
			Scene2D* scene;
			PhysicalObjectCopy2D A;
			PhysicalObjectCopy2D B;
			ContactManifold2D contactManifold;

			// TODO: Differentiate static friction from dynamic friction.
			float32 combinedFriction;
	};
}