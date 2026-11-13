// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Physics2D/Layer/SceneLayer.hpp"

namespace Trident {
	void Physics2DSceneLayer::LoadDominoSample(PhysicsSimulation2DDescriptor& simulationDescriptor) {
		rendererOptions.hulls = sequence<PhysicsRendererBuildHull2D>(6);
		rendererOptions.maxCircleInstanceCount = 0;

		simulationDescriptor.maxStaticRigidBodyCount = 4;
		simulationDescriptor.maxKinematicAndDynamicRigidBodyCount = 11;
		simulationDescriptor.maxStaticShapeCount = 4;
		simulationDescriptor.maxKinematicAndDynamicShapeCount = 11;

		simulation = createSimulation(simulationDescriptor);

		boxHulls = {
			createBoxHull({50.0f, 10.0f}),
			createBoxHull({6.0f, 0.25f}),
			createBoxHull({7.0f, 0.25f}),
			createBoxHull({0.25f, 1.5f}),
			createBoxHull({0.25f, 1.0f}),
			createBoxHull({0.1f, 1.0f}),
		};

		registerHullForRender(&boxHulls[0].header);
		registerHullForRender(&boxHulls[1].header);
		registerHullForRender(&boxHulls[2].header);
		registerHullForRender(&boxHulls[3].header);
		registerHullForRender(&boxHulls[4].header);
		registerHullForRender(&boxHulls[5].header);

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
				.position = {-1.5f, 10.0f},
			}),
			.material = material,
			.density = 1.0f,
		}, &boxHulls[1]);

		// Obstacle 2
		createShape(simulation, {
			.type = ShapeType2D::CONVEX_HULL,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::STATIC,
				.position = {1.0f, 6.0f},
				.rotation = 0.3f,
			}),
			.material = material,
			.density = 1.0f,
		}, &boxHulls[2]);

		// Obstacle 3
		createShape(simulation, {
			.type = ShapeType2D::CONVEX_HULL,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::STATIC,
				.position = {-7.0f, 4.0f},
			}),
			.material = material,
			.density = 1.0f,
		}, &boxHulls[3]);

		// Pusher
		createShape(simulation, {
			.type = ShapeType2D::CONVEX_HULL,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::DYNAMIC,
				.position = {-7.5f, 12.0f},
				.linearVelocity = {4.0f, 0.0f},
			}),
			.material = material,
			.density = 10.0f,
		}, &boxHulls[4]);

		material.friction = 0.1f;

		uint32 i = 0;

		while (i < 10) {
			createShape(simulation, {
				.type = ShapeType2D::CONVEX_HULL,
				.rigidBodyId = createRigidBody(simulation, {
					.type = RigidBodyType::DYNAMIC,
					.position = {i - 6.0f, 11.125f},
				}),
				.material = material,
				.density = 10.0f,
			}, &boxHulls[5]);

			i += 1;
		}
	}
}