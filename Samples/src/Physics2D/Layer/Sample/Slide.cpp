// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2D/Layer/SceneLayer.hpp"

namespace Trident {
	void Physics2DSceneLayer::LoadSlideSample(PhysicsSimulationDescriptor2D& simulationDescriptor) {
		rendererBuildOptions.hulls = sequence<PhysicsRendererBuildHull2D>(3);
		rendererBuildOptions.maxCircleInstanceCount = 1;

		simulationDescriptor.maxStaticRigidBodyCount = 3;
		simulationDescriptor.maxKinematicAndDynamicRigidBodyCount = 1;
		simulationDescriptor.maxStaticShapeCount = 3;
		simulationDescriptor.maxKinematicAndDynamicShapeCount = 1;

		simulation = createSimulation(simulationDescriptor);

		boxHulls = {
			createBoxHull({50.0f, 10.0f}),
			createBoxHull({10.0f, 0.25f}),
			createBoxHull({2.5f, 0.25f}),
		};

		registerHullForRender(&boxHulls[0].header);
		registerHullForRender(&boxHulls[1].header);
		registerHullForRender(&boxHulls[2].header);

		PhysicsMaterial material;
		material.friction = 0.2f;

		CircleShape2D circleShape;
		circleShape.radius = 0.5f;

		// Floor
		createShape(simulation, {
			.type = ShapeType2D::CONVEX_HULL,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::STATIC,
				.position = {0.0f, -10.0f},
			}),
			.material = material,
			.density = 1.0f,
		}, &boxHulls[0]);

		// Slide 1
		createShape(simulation, {
			.type = ShapeType2D::CONVEX_HULL,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::STATIC,
				.position = {-4.0f, 3.2f},
				.rotation = radians(20.0_deg),
			}),
			.material = material,
			.density = 1.0f,
		}, &boxHulls[1]);

		// Slide 2
		createShape(simulation, {
			.type = ShapeType2D::CONVEX_HULL,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::STATIC,
				.position = {8.0f, 0.5f},
				.rotation = radians(-20.0_deg),
			}),
			.material = material,
			.density = 1.0f,
		}, &boxHulls[2]);

		// Ball
		createShape(simulation, {
			.type = ShapeType2D::CIRCLE,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::DYNAMIC,
				.position = {-13.0f, 10.0f},
			}),
			.material = material,
			.density = 25.0f,
		}, &circleShape);
	}
}