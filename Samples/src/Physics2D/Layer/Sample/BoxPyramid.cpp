// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Physics2D/Layer/SceneLayer.hpp"

namespace Trident {
	void Physics2DSceneLayer::LoadBoxPyramidSample(PhysicsSimulation2DDescriptor& simulationDescriptor) {
		rendererOptions.hulls = sequence<PhysicsRendererBuildHull2D>(2);
		rendererOptions.maxCircleInstanceCount = 0;

		simulationDescriptor.maxStaticRigidBodyCount = 1;
		simulationDescriptor.maxKinematicAndDynamicRigidBodyCount = 78;
		simulationDescriptor.maxStaticShapeCount = 1;
		simulationDescriptor.maxKinematicAndDynamicShapeCount = 78;

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

		vec2<float32> x = {-6.0f, 0.75f};
		uint32 i = 0;

		while (i < 12) {
			vec2<float32> y = x;
			uint32 j = i;

			while (j < 12) {
				createShape(simulation, {
					.type = ShapeType2D::CONVEX_HULL,
					.rigidBodyId = createRigidBody(simulation, {
						.type = RigidBodyType::DYNAMIC,
						.position = y,
					}),
					.material = material,
					.density = 10.0f,
				}, &boxHulls[1]);

				y += {1.125_m, 0.0_m};
				j += 1;
			}

			x += {0.5625_m, 2.0_m};
			i += 1;
		}
	}
}