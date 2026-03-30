// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Constraint/Constraint2D.hpp"
#include "Engine/API/Physics/RigidBody2D.hpp"

namespace Trident {
	struct RotationConstraint2DDescriptor {
		Entity A;
	};

	class RotationConstraint2D : public Constraint2D {
		public:
			explicit RotationConstraint2D(const RotationConstraint2DDescriptor& descriptor) :
				A(descriptor.A)
			{}

			void update(ECS* ecs, float32 deltaTime) const final override {
				RigidBody2D& rigidBodyA = ecs->getComponent<RigidBody2D>(A);
				rigidBodyA.angularVelocity = 0.0f;
			}

		private:
			Entity A;
	};
}