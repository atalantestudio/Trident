// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Constraint/PointConstraint2D.hpp"

#include "../BaseLayer.hpp"

namespace Trident {
	class Physics2DConstraintLayer : public BaseLayer {
		public:
			void build() override {
				Scene2D& scene = pointer_cast<SampleApplication*>(getApplication())->scene;

				const ref<ConvexHullGeometry2D> squareGeometry = ConvexHullGeometry2D::unitSquare();

				RigidBody2DDescriptor boxDescriptor{};
				boxDescriptor.position = {0.0_m, 0.0_m};
				boxDescriptor.scale = {2.0f, 2.0f};
				boxDescriptor.mass = 1.0_kg;
				boxDescriptor.friction = 0.2f;
				boxDescriptor.geometry = squareGeometry;
				boxDescriptor.fillColor = 0xf4fedbff;

				scene.createGhostRigidBody(boxDescriptor);

				boxDescriptor.position = {0.0_m, 5.0_m};
				boxDescriptor.scale = {1.0f, 1.0f};
				boxDescriptor.fillColor = 0xe6ddffff;

				scene.createGhostRigidBody(boxDescriptor);

				PointConstraint2DDescriptor constraintDescriptor{};
				constraintDescriptor.A = 0;
				constraintDescriptor.B = 1;

				scene.createConstraint<PointConstraint2D>(constraintDescriptor);
			}
	};
}