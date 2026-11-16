// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Physics2D/Layer/SceneLayer.hpp"

namespace Trident {
	static BoxHull2D createPolygonHull() {
		static constexpr vec2<float32> vertices[] = {
			{-3.0f,   0.0f},
			{ 4.0f,   0.0f},
			{ 3.0f,  -1.0f},
			{-2.25f, -1.5f},
		};

		return createHull(vertices);
	}

	static BoxHull2D createTriangleHull() {
		static constexpr vec2<float32> vertices[] = {
			{0.0f, 0.0f},
			{0.0f, 1.0f},
			{6.0f, 0.0f},
		};

		return createHull(vertices);
	}

	void Physics2DSceneLayer::LoadPolygonSample(PhysicsSimulationDescriptor2D& simulationDescriptor) {
		rendererBuildOptions.hulls = sequence<PhysicsRendererBuildHull2D>(3);
		rendererBuildOptions.maxCircleInstanceCount = 0;

		simulationDescriptor.maxStaticRigidBodyCount = 2;
		simulationDescriptor.maxKinematicAndDynamicRigidBodyCount = 1;
		simulationDescriptor.maxStaticShapeCount = 2;
		simulationDescriptor.maxKinematicAndDynamicShapeCount = 1;

		simulation = createSimulation(simulationDescriptor);

		boxHulls = {
			createBoxHull({50.0f, 10.0f}),
			createPolygonHull(),
			createTriangleHull(),
		};

		registerHullForRender(&boxHulls[0].header);
		registerHullForRender(&boxHulls[1].header);
		registerHullForRender(&boxHulls[2].header);

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

		// Polygon
		createShape(simulation, {
			.type = ShapeType2D::CONVEX_HULL,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::STATIC,
				.position = {3.0f, 4.0f},
			}),
			.material = material,
			.density = 1.0f,
		}, &boxHulls[1]);

		// Triangle
		createShape(simulation, {
			.type = ShapeType2D::CONVEX_HULL,
			.rigidBodyId = createRigidBody(simulation, {
				.type = RigidBodyType::DYNAMIC,
				.position = {0.0f, 8.0f},
				.rotation = radians(-165.0_deg),
			}),
			.material = material,
			.density = 1.0f,
		}, &boxHulls[2]);
	}
}