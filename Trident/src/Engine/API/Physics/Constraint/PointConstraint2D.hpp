// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Constraint/Constraint2D.hpp"

namespace Trident {
	struct PointConstraint2DDescriptor {
		Entity A;
		Entity B;
	};

	class PointConstraint2D : public Constraint2D {
		public:
			explicit PointConstraint2D(const PointConstraint2DDescriptor& descriptor) :
				A(descriptor.A),
				B(descriptor.B)
			{}

			void update(Scene2D* scene, float32 deltaTime) const final override {
				static constexpr float32 DAMPING_COEFFICIENT = 0.9f;

				const RigidBody2D& rigidBodyA = scene->getComponent<RigidBody2D>(A);
				RigidBody2D& rigidBodyB = scene->getComponent<RigidBody2D>(B);

				const vec2<float32> C = rigidBodyB.position - rigidBodyA.position;

				rigidBodyB.linearVelocity += -C * (beta / deltaTime);
				rigidBodyB.linearVelocity *= DAMPING_COEFFICIENT;
			}

		private:
			Entity A;
			Entity B;
			float32 beta = 0.01f;
	};
}