// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Physics/2D/Joint2D.hpp"
#include "Engine/API/Physics/2D/Simulation.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"

namespace Trident {
	void updateJoints(PhysicsSimulation2D& simulation) {
		const ComponentPool<Joint2D>& jointPool = simulation.scene->ecs.getComponentPool<Joint2D>();
		uint32 componentIndex = 0;

		while (componentIndex < jointPool.componentCount) {
			const Entity entity = jointPool.componentIndexToEntity[componentIndex];

			Joint2D& joint = simulation.scene->ecs.getComponent<Joint2D>(entity);
			RigidBody2DComponent& rigidBodyA = simulation.scene->ecs.getComponent<RigidBody2DComponent>(joint.A);
			RigidBody2DComponent& rigidBodyB = simulation.scene->ecs.getComponent<RigidBody2DComponent>(joint.B);

			// TODO(physics/2d/update-joints): Why is the transpose needed?
			const mat2x2<float32> RAT = transpose(rotationAngle<mat2x2<float32>>(rigidBodyA.rotation));
			const mat2x2<float32> RBT = transpose(rotationAngle<mat2x2<float32>>(rigidBodyB.rotation));

			joint.rA = joint.localAnchorA * RAT;
			joint.rB = joint.localAnchorB * RBT;

			const vec2<float32> pA = rigidBodyA.position + joint.rA;
			const vec2<float32> pB = rigidBodyB.position + joint.rB;
			const vec2<float32> pAB = pB - pA;

			const mat2x2<float32> K1 = {
				rigidBodyA.inverseMass + rigidBodyB.inverseMass, 0.0f,
				0.0f,                                            rigidBodyA.inverseMass + rigidBodyB.inverseMass,
			};
			const mat2x2<float32> K2 = {
				 rigidBodyA.inverseInertia * joint.rA[1] * joint.rA[1], -rigidBodyA.inverseInertia * joint.rA[0] * joint.rA[1],
				-rigidBodyA.inverseInertia * joint.rA[0] * joint.rA[1],  rigidBodyA.inverseInertia * joint.rA[0] * joint.rA[0],
			};
			const mat2x2<float32> K3 = {
				 rigidBodyB.inverseInertia * joint.rB[1] * joint.rB[1], -rigidBodyB.inverseInertia * joint.rB[0] * joint.rB[1],
				-rigidBodyB.inverseInertia * joint.rB[0] * joint.rB[1],  rigidBodyB.inverseInertia * joint.rB[0] * joint.rB[0],
			};
			const mat2x2<float32> S = {
				joint.softness, 0.0f,
				0.0f,           joint.softness,
			};
			const mat2x2<float32> K = K1 + K2 + K3 + S;

			joint.M = inverse(K);
			joint.bias = pAB * simulation.inverseDeltaTime * -joint.biasFactor;

			#if defined(ENABLE_PHYSICS_2D_WARM_STARTING)
				rigidBodyA.applyImpulse(-joint.accumulatedImpulse, joint.rA);
				rigidBodyB.applyImpulse(joint.accumulatedImpulse, joint.rB);
			#else
				joint.accumulatedImpulse = {0.0f, 0.0f};
			#endif

			componentIndex += 1;
		}
	}
}