// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Physics2D/Layer/SceneLayer.hpp"

namespace Trident {
	void Physics2DSceneLayer::LoadSwingSample(PhysicsSimulation2DDescriptor& simulationDescriptor) {
		rendererOptions.hulls = sequence<PhysicsRendererBuildHull2D>(5);
		rendererOptions.maxCircleInstanceCount = 0;

		simulationDescriptor.maxStaticRigidBodyCount = 2;
		simulationDescriptor.maxKinematicAndDynamicRigidBodyCount = 4;
		simulationDescriptor.maxStaticShapeCount = 2;
		simulationDescriptor.maxKinematicAndDynamicShapeCount = 4;

		simulation = createSimulation(simulationDescriptor);

		boxHulls = {
			createBoxHull({50.0f, 10.0f}),
			createBoxHull({0.05f, 0.5f}),
			createBoxHull({6.0f, 0.125f}),
			createBoxHull({0.25f, 0.25f}),
			createBoxHull({0.5f, 0.5f}),
		};

		registerHullForRender(&boxHulls[0].header);
		registerHullForRender(&boxHulls[1].header);
		registerHullForRender(&boxHulls[2].header);
		registerHullForRender(&boxHulls[3].header);
		registerHullForRender(&boxHulls[4].header);

		PhysicsMaterial material;
		material.friction = 0.2f;

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

		material.friction = 0.0f;

		// Swing base
		createShape(simulation, {
			.type = ShapeType2D::CONVEX_HULL,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::STATIC,
				.position = {0.0f, 0.5f},
			}),
			.material = material,
			.density = 1.0f,
		}, &boxHulls[1]);

		material.friction = 0.2f;

		// Swing
		createShape(simulation, {
			.type = ShapeType2D::CONVEX_HULL,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::DYNAMIC,
				.position = {0.0f, 1.0f},
			}),
			.material = material,
			.density = 100.0f,
		}, &boxHulls[2]);

		// Small weight 1
		createShape(simulation, {
			.type = ShapeType2D::CONVEX_HULL,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::DYNAMIC,
				.position = {-5.0f, 2.0f},
			}),
			.material = material,
			.density = 25.0f,
		}, &boxHulls[3]);

		// Small weight 2
		createShape(simulation, {
			.type = ShapeType2D::CONVEX_HULL,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::DYNAMIC,
				.position = {-5.5f, 2.0f},
			}),
			.material = material,
			.density = 25.0f,
		}, &boxHulls[3]);

		// Large weight
		createShape(simulation, {
			.type = ShapeType2D::CONVEX_HULL,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::DYNAMIC,
				.position = {5.5f, 15.0f},
			}),
			.material = material,
			.density = 100.0f,
		}, &boxHulls[4]);
	}
}