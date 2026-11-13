// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Physics2D/Layer/SceneLayer.hpp"

namespace Trident {
	void Physics2DSceneLayer::LoadBoxStackingSample(PhysicsSimulation2DDescriptor& simulationDescriptor) {
		rendererOptions.hulls = sequence<PhysicsRendererBuildHull2D>(2);
		rendererOptions.maxCircleInstanceCount = 0;

		simulationDescriptor.maxStaticRigidBodyCount = 1;
		simulationDescriptor.maxKinematicAndDynamicRigidBodyCount = 10;
		simulationDescriptor.maxStaticShapeCount = 1;
		simulationDescriptor.maxKinematicAndDynamicShapeCount = 10;

		simulation = createSimulation(simulationDescriptor);

		boxHulls = {
			createBoxHull({50.0f, 10.0f}),
			createBoxHull({0.5f, 0.5f}),
		};

		registerHullForRender(&boxHulls[0].header);
		registerHullForRender(&boxHulls[1].header);

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

		uint32 i = 0;

		while (i < 10) {
			createShape(simulation, {
				.type = ShapeType2D::CONVEX_HULL,
				.rigidBodyId = createRigidBody(simulation, {
					.type = RigidBodyType::DYNAMIC,
					.position = {0.0f, 1.05f * i + 0.51f},
				}),
				.material = material,
				.density = 1.0f,
			}, &boxHulls[1]);

			i += 1;
		}
	}
}