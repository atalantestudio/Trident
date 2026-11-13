// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Physics2D/Layer/SceneLayer.hpp"

namespace Trident {
	void Physics2DSceneLayer::LoadFrictionSample(PhysicsSimulation2DDescriptor& simulationDescriptor) {
		static constexpr float32 FRICTIONS[] = {0.75f, 0.5f, 0.35f, 0.1f, 0.0f};

		rendererOptions.hulls = sequence<PhysicsRendererBuildHull2D>(4);
		rendererOptions.maxCircleInstanceCount = 0;

		simulationDescriptor.maxStaticRigidBodyCount = 6;
		simulationDescriptor.maxKinematicAndDynamicRigidBodyCount = 5;
		simulationDescriptor.maxStaticShapeCount = 6;
		simulationDescriptor.maxKinematicAndDynamicShapeCount = 5;

		simulation = createSimulation(simulationDescriptor);

		boxHulls = {
			createBoxHull({50.0f, 10.0f}),
			createBoxHull({6.5f, 0.125f}),
			createBoxHull({0.125f, 0.5f}),
			createBoxHull({0.25f, 0.25f}),
		};

		registerHullForRender(&boxHulls[0].header);
		registerHullForRender(&boxHulls[1].header);
		registerHullForRender(&boxHulls[2].header);
		registerHullForRender(&boxHulls[3].header);

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

		// Obstacle 1
		createShape(simulation, {
			.type = ShapeType2D::CONVEX_HULL,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::STATIC,
				.position = {-2.0f, 11.0f},
				.rotation = -0.25f,
			}),
			.material = material,
			.density = 1.0f,
		}, &boxHulls[1]);

		// Obstacle 2
		createShape(simulation, {
			.type = ShapeType2D::CONVEX_HULL,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::STATIC,
				.position = {5.25f, 9.5f},
			}),
			.material = material,
			.density = 1.0f,
		}, &boxHulls[2]);

		// Obstacle 3
		createShape(simulation, {
			.type = ShapeType2D::CONVEX_HULL,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::STATIC,
				.position = {2.0f, 7.0f},
				.rotation = 0.25f,
			}),
			.material = material,
			.density = 1.0f,
		}, &boxHulls[1]);

		// Obstacle 4
		createShape(simulation, {
			.type = ShapeType2D::CONVEX_HULL,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::STATIC,
				.position = {-5.25f, 5.5f},
			}),
			.material = material,
			.density = 1.0f,
		}, &boxHulls[2]);

		// Obstacle 5
		createShape(simulation, {
			.type = ShapeType2D::CONVEX_HULL,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::STATIC,
				.position = {-2.0f, 3.0f},
				.rotation = -0.25f,
			}),
			.material = material,
			.density = 1.0f,
		}, &boxHulls[1]);

		uint32 i = 0;

		while (i < 5) {
			material.friction = FRICTIONS[i];

			createShape(simulation, {
				.type = ShapeType2D::CONVEX_HULL,
				.rigidBodyId = createRigidBody(simulation, {
					.type = RigidBodyType::DYNAMIC,
					.position = {2.0f * i - 7.5f, 14.0f},
				}),
				.material = material,
				.density = 25.0f,
			}, &boxHulls[3]);

			i += 1;
		}
	}
}