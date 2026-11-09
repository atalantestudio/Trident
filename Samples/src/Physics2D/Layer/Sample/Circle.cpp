// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Physics2D/Layer/SceneLayer.hpp"

namespace Trident {
	void Physics2DSceneLayer::LoadCircleSample(PhysicsSimulation2DDescriptor& simulationDescriptor) {
		rendererOptions.hulls = {};
		rendererOptions.maxCircleInstanceCount = 3;

		simulationDescriptor.maxStaticRigidBodyCount = 2;
		simulationDescriptor.maxKinematicAndDynamicRigidBodyCount = 1;
		simulationDescriptor.maxStaticShapeCount = 2;
		simulationDescriptor.maxKinematicAndDynamicShapeCount = 1;

		simulation = createSimulation(simulationDescriptor);

		PhysicsMaterial material;
		material.friction = 0.2f;

		CircleShape2D circleShape;
		circleShape.radius = 10.0f;

		// Static circle 1
		createShape(simulation, {
			.type = ShapeType2D::CIRCLE,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::STATIC,
				.flags = {},
				.position = {-10.0f, -10.0f},
				.rotation = 0.0f,
			}),
			.material = material,
			.density = 1.0f,
		}, &circleShape);

		// Static circle 2
		createShape(simulation, {
			.type = ShapeType2D::CIRCLE,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::STATIC,
				.flags = {},
				.position = {10.0f, -10.0f},
				.rotation = 0.0f,
			}),
			.material = material,
			.density = 1.0f,
		}, &circleShape);

		circleShape.radius = 0.5f;

		// Dynamic circle
		createShape(simulation, {
			.type = ShapeType2D::CIRCLE,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::DYNAMIC,
				.flags = {},
				.position = {9.5f, 1.0f},
				.rotation = 0.0f,
			}),
			.material = material,
			.density = 200.0f,
		}, &circleShape);
	}
}