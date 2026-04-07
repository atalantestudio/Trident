// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "RigidBody2DComponent.hpp"

namespace Trident {
	// TODO(debug-only)
	constexpr uint32 UNIT_CIRCLE_GEOMETRY_ASSET_IDENTIFIER = 34;
	constexpr uint32 UNIT_SQUARE_GEOMETRY_ASSET_IDENTIFIER = 35;

	Scene2DData createBoxesScene() {
		return {
			.staticVertexCount = 0,
			.staticIndexCount = 0,
			.entityCount = 2,
			.geometryAssetIdentifiers = {UNIT_SQUARE_GEOMETRY_ASSET_IDENTIFIER},
			.geometryMappings = {
				{0, {0}},
				{1, {0}},
			},
			.rigidBodyMappings = {
				// Floor
				{
					0,
					createRigidBody({
						.position = {0.0_m, -10.0_m},
						.rotation = 0.0_rad,
						.scale = {100.0f, 20.0f},
						.mass = max<float32>(),
						.friction = 0.2f,
						.restitution = 0.0f,
						.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
					}),
				},

				// Box
				{
					1,
					createRigidBody({
						.position = {0.0_m, 4.0_m},
						.rotation = 0.0_rad,
						.scale = {1.0f, 1.0f},
						.mass = 200.0_kg,
						.friction = 0.2f,
						.restitution = 0.0f,
						.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
					}),
				},
			},
		};
	}

	Scene2DData createCirclesScene() {
		return {
			.staticVertexCount = 0,
			.staticIndexCount = 0,
			.entityCount = 3,
			.geometryAssetIdentifiers = {UNIT_CIRCLE_GEOMETRY_ASSET_IDENTIFIER},
			.geometryMappings = {
				{0, {0}},
				{1, {0}},
				{2, {0}},
			},
			.rigidBodyMappings = {
				// Static circle 1
				{
					0,
					createRigidBody({
						.position = {-10.0_m, -10.0_m},
						.rotation = 0.0_rad,
						.scale = {20.0f, 20.0f},
						.mass = max<float32>(),
						.friction = 0.2f,
						.restitution = 0.0f,
						.momentOfInertia = MomentOfInertia2D::THIN_SOLID_DISK,
					}),
				},

				// Static circle 2
				{
					1,
					createRigidBody({
						.position = {10.0_m, -10.0_m},
						.rotation = 0.0_rad,
						.scale = {20.0f, 20.0f},
						.mass = max<float32>(),
						.friction = 0.2f,
						.restitution = 0.0f,
						.momentOfInertia = MomentOfInertia2D::THIN_SOLID_DISK,
					}),
				},

				// Dynamic circle
				{
					2,
					createRigidBody({
						.position = {9.5_m, 1.0_m},
						.rotation = 0.0_rad,
						.scale = {1.0f, 1.0f},
						.mass = 200.0_kg,
						.friction = 0.2f,
						.restitution = 0.0f,
						.momentOfInertia = MomentOfInertia2D::THIN_SOLID_DISK,
					}),
				},
			},
		};
	}

	Scene2DData createDominosScene() {
		constexpr uint32 entityCount = 15;

		sequence<EntityComponentMapping<GeometryComponent>> geometryMappings(entityCount);

		Entity entity = 0;

		while (entity < geometryMappings.count()) {
			geometryMappings[entity] = {entity, {0}};

			entity += 1;
		}

		sequence<EntityComponentMapping<RigidBody2DComponent>> rigidBodyMappings(entityCount);

		// Floor
		rigidBodyMappings[0] = {
			0,
			createRigidBody({
				.position = {0.0_m, -10.0_m},
				.rotation = 0.0_rad,
				.scale = {100.0f, 20.0f},
				.mass = max<float32>(),
				.friction = 0.2f,
				.restitution = 0.0f,
				.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
			}),
		};

		// Obstacle 1
		rigidBodyMappings[1] = {
			1,
			createRigidBody({
				.position = {-1.5_m, 10.0_m},
				.rotation = 0.0_rad,
				.scale = {12.0f, 0.5f},
				.mass = max<float32>(),
				.friction = 0.2f,
				.restitution = 0.0f,
				.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
			}),
		};

		// Obstacle 2
		rigidBodyMappings[2] = {
			2,
			createRigidBody({
				.position = {1.0_m, 6.0_m},
				.rotation = 0.3_rad,
				.scale = {14.0f, 0.5f},
				.mass = max<float32>(),
				.friction = 0.2f,
				.restitution = 0.0f,
				.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
			}),
		};

		// Obstacle 3
		rigidBodyMappings[3] = {
			3,
			createRigidBody({
				.position = {-7.0_m, 4.0_m},
				.rotation = 0.0_rad,
				.scale = {0.5f, 3.0f},
				.mass = max<float32>(),
				.friction = 0.2f,
				.restitution = 0.0f,
				.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
			}),
		};

		// Pusher
		rigidBodyMappings[4] = {
			4,
			createRigidBody({
				.position = {-7.5_m, 12.0_m},
				.rotation = 0.0_rad,
				.scale = {0.5f, 2.0f},
				.linearVelocity = {4.0_m_s, 0.0_m_s},
				.mass = 10.0_kg,
				.friction = 0.2f,
				.restitution = 0.0f,
				.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
			}),
		};

		Descriptor<RigidBody2DComponent> dominoDescriptor = {
			.rotation = 0.0_rad,
			.scale = {0.2_m, 2.0_m},
			.mass = 10.0_kg,
			.friction = 0.1f,
			.restitution = 0.0f,
			.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
		};

		uint32 dominoIndex = 0;

		while (dominoIndex < 10) {
			dominoDescriptor.position = {-6.0_m + dominoIndex, 11.125_m};

			rigidBodyMappings[dominoIndex + 5] = {dominoIndex + 5, createRigidBody(dominoDescriptor)};

			dominoIndex += 1;
			entity += 1;
		}

		return {
			.staticVertexCount = 0,
			.staticIndexCount = 0,
			.entityCount = entityCount,
			.geometryAssetIdentifiers = {UNIT_SQUARE_GEOMETRY_ASSET_IDENTIFIER},
			.geometryMappings = geometryMappings,
			.rigidBodyMappings = rigidBodyMappings,
		};
	}

	Scene2DData createPyramidScene() {
		constexpr uint32 entityCount = 79;

		sequence<EntityComponentMapping<GeometryComponent>> geometryMappings(entityCount);

		Entity entity = 0;

		while (entity < geometryMappings.count()) {
			geometryMappings[entity] = {entity, {0}};

			entity += 1;
		}

		sequence<EntityComponentMapping<RigidBody2DComponent>> rigidBodyMappings(entityCount);

		// Floor
		rigidBodyMappings[0] = {
			0,
			createRigidBody({
				.position = {0.0_m, -10.0_m},
				.rotation = 0.0_rad,
				.scale = {100.0f, 20.0f},
				.mass = max<float32>(),
				.friction = 0.2f,
				.restitution = 0.0f,
				.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
			}),
		};

		Descriptor<RigidBody2DComponent> boxDescriptor = {
			.rotation = 0.0_rad,
			.scale = {1.0f, 1.0f},
			.mass = 10.0_kg,
			.friction = 0.2f,
			.restitution = 0.0f,
			.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
		};

		vec2<float32> x = {-6.0_m, 0.75_m};
		vec2<float32> y;
		uint32 i = 0;
		uint32 j;

		entity = 1;

		while (i < 12) {
			y = x;
			j = i;

			while (j < 12) {
				boxDescriptor.position = svec2<float32>(y);

				rigidBodyMappings[entity] = {entity, createRigidBody(boxDescriptor)};

				entity += 1;

				y += {1.125_m, 0.0_m};
				j += 1;
			}

			x += {0.5625_m, 2.0_m};
			i += 1;
		}

		return {
			.staticVertexCount = 0,
			.staticIndexCount = 0,
			.entityCount = entityCount,
			.geometryAssetIdentifiers = {UNIT_SQUARE_GEOMETRY_ASSET_IDENTIFIER},
			.geometryMappings = geometryMappings,
			.rigidBodyMappings = rigidBodyMappings,
		};
	}

	Scene2DData createRollingBallScene() {
		return {
			.staticVertexCount = 0,
			.staticIndexCount = 0,
			.entityCount = 4,
			.geometryAssetIdentifiers = {
				UNIT_SQUARE_GEOMETRY_ASSET_IDENTIFIER,
				UNIT_CIRCLE_GEOMETRY_ASSET_IDENTIFIER,
			},
			.geometryMappings = {
				{0, {0}},
				{1, {0}},
				{2, {0}},
				{3, {1}},
			},
			.rigidBodyMappings = {
				// Floor
				{
					0,
					createRigidBody({
						.position = {0.0_m, -10.0_m},
						.rotation = 0.0_rad,
						.scale = {100.0f, 20.0f},
						.mass = max<float32>(),
						.friction = 0.2f,
						.restitution = 0.0f,
						.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
					}),
				},

				// Slide 1
				{
					1,
					createRigidBody({
						.position = {-4.0_m, 3.2_m},
						.rotation = radians(-20.0_deg),
						.scale = {20.0f, 0.5f},
						.mass = max<float32>(),
						.friction = 0.2f,
						.restitution = 0.0f,
						.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
					}),
				},

				// Slide 2
				{
					2,
					createRigidBody({
						.position = {8.0_m, 0.5_m},
						.rotation = radians(20.0_deg),
						.scale = {5.0f, 0.5f},
						.mass = max<float32>(),
						.friction = 0.2f,
						.restitution = 0.0f,
						.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
					}),
				},

				// Ball
				{
					3,
					createRigidBody({
						.position = {-13.0_m, 10.0_m},
						.rotation = 0.0_rad,
						.scale = {1.0f, 1.0f},
						.mass = 25.0_kg,
						.friction = 0.2f,
						.restitution = 0.0f,
						.momentOfInertia = MomentOfInertia2D::THIN_SOLID_DISK,
					}),
				},
			},
		};
	}

	Scene2DData createSlidingBoxesScene() {
		constexpr uint32 entityCount = 11;
		constexpr float32 frictions[] = {0.75f, 0.5f, 0.35f, 0.1f, 0.0f};

		sequence<EntityComponentMapping<GeometryComponent>> geometryMappings(entityCount);

		Entity entity = 0;

		while (entity < geometryMappings.count()) {
			geometryMappings[entity] = {entity, {0}};

			entity += 1;
		}

		sequence<EntityComponentMapping<RigidBody2DComponent>> rigidBodyMappings(entityCount);

		// Floor
		rigidBodyMappings[0] = {
			0,
			createRigidBody({
				.position = {0.0_m, -10.0_m},
				.rotation = 0.0_rad,
				.scale = {100.0f, 20.0f},
				.mass = max<float32>(),
				.friction = 0.2f,
				.restitution = 0.0f,
				.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
			}),
		};

		// Obstacle 1
		rigidBodyMappings[1] = {
			1,
			createRigidBody({
				.position = {-2.0_m, 11.0_m},
				.rotation = -0.25_rad,
				.scale = {13.0f, 0.25f},
				.mass = max<float32>(),
				.friction = 0.2f,
				.restitution = 0.0f,
				.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
			}),
		};

		// Obstacle 2
		rigidBodyMappings[2] = {
			2,
			createRigidBody({
				.position = {5.25_m, 9.5_m},
				.rotation = 0.0_rad,
				.scale = {0.25f, 1.0f},
				.mass = max<float32>(),
				.friction = 0.2f,
				.restitution = 0.0f,
				.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
			}),
		};

		// Obstacle 3
		rigidBodyMappings[3] = {
			3,
			createRigidBody({
				.position = {2.0_m, 7.0_m},
				.rotation = 0.25_rad,
				.scale = {13.0f, 0.25f},
				.mass = max<float32>(),
				.friction = 0.2f,
				.restitution = 0.0f,
				.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
			}),
		};

		// Obstacle 4
		rigidBodyMappings[4] = {
			4,
			createRigidBody({
				.position = {-5.25_m, 5.5_m},
				.rotation = 0.0_rad,
				.scale = {0.25f, 1.0f},
				.mass = max<float32>(),
				.friction = 0.2f,
				.restitution = 0.0f,
				.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
			}),
		};

		// Obstacle 5
		rigidBodyMappings[5] = {
			5,
			createRigidBody({
				.position = {-2.0_m, 3.0_m},
				.rotation = -0.25_rad,
				.scale = {13.0f, 0.25f},
				.mass = max<float32>(),
				.friction = 0.2f,
				.restitution = 0.0f,
				.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
			}),
		};

		Descriptor<RigidBody2DComponent> boxDescriptor = {
			.rotation = 0.0_rad,
			.scale = {0.5f, 0.5f},
			.mass = 25.0_kg,
			.friction = 0.2f,
			.restitution = 0.0f,
			.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
		};

		uint32 boxIndex = 0;

		while (boxIndex < 5) {
			boxDescriptor.position = {-7.5_m + boxIndex * 2.0f, 14.0_m};
			boxDescriptor.friction = frictions[boxIndex];

			rigidBodyMappings[boxIndex + 6] = {boxIndex + 6, createRigidBody(boxDescriptor)};

			boxIndex += 1;
		}

		return {
			.staticVertexCount = 0,
			.staticIndexCount = 0,
			.entityCount = entityCount,
			.geometryAssetIdentifiers = {UNIT_SQUARE_GEOMETRY_ASSET_IDENTIFIER},
			.geometryMappings = geometryMappings,
			.rigidBodyMappings = rigidBodyMappings,
		};
	}

	Scene2DData createStackingScene() {
		constexpr uint32 entityCount = 11;

		sequence<EntityComponentMapping<GeometryComponent>> geometryMappings(entityCount);

		Entity entity = 0;

		while (entity < geometryMappings.count()) {
			geometryMappings[entity] = {entity, {0}};

			entity += 1;
		}

		sequence<EntityComponentMapping<RigidBody2DComponent>> rigidBodyMappings(entityCount);

		// Floor
		rigidBodyMappings[0] = {
			0,
			createRigidBody({
				.position = {0.0_m, -10.0_m},
				.rotation = 0.0_rad,
				.scale = {100.0f, 20.0f},
				.mass = max<float32>(),
				.friction = 0.2f,
				.restitution = 0.0f,
				.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
			}),
		};

		Descriptor<RigidBody2DComponent> boxDescriptor = {
			.rotation = 0.0_rad,
			.scale = {1.0f, 1.0f},
			.mass = 10.0_kg,
			.friction = 0.2f,
			.restitution = 0.0f,
			.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
		};

		uint32 boxIndex = 0;

		while (boxIndex < 10) {
			boxDescriptor.position = {0.0_m, 0.51_m + boxIndex * 1.05f};

			rigidBodyMappings[boxIndex + 1] = {boxIndex + 1, createRigidBody(boxDescriptor)};

			boxIndex += 1;
		}

		return {
			.staticVertexCount = 0,
			.staticIndexCount = 0,
			.entityCount = entityCount,
			.geometryAssetIdentifiers = {UNIT_SQUARE_GEOMETRY_ASSET_IDENTIFIER},
			.geometryMappings = geometryMappings,
			.rigidBodyMappings = rigidBodyMappings,
		};
	}

	Scene2DData createSwingScene() {
		return {
			.staticVertexCount = 0,
			.staticIndexCount = 0,
			.entityCount = 6,
			.geometryAssetIdentifiers = {UNIT_SQUARE_GEOMETRY_ASSET_IDENTIFIER},
			.geometryMappings = {
				{0, {0}},
				{1, {0}},
				{2, {0}},
				{3, {0}},
				{4, {0}},
				{5, {0}},
			},
			.rigidBodyMappings = {
				// Floor
				{
					0,
					createRigidBody({
						.position = {0.0_m, -10.0_m},
						.rotation = 0.0_rad,
						.scale = {100.0f, 20.0f},
						.mass = max<float32>(),
						.friction = 0.2f,
						.restitution = 0.0f,
						.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
					}),
				},

				// Swing base
				{
					1,
					createRigidBody({
						.position = {0.0_m, 0.5_m},
						.rotation = 0.0_rad,
						.scale = {0.1f, 1.0f},
						.mass = max<float32>(),
						.friction = 0.0f,
						.restitution = 0.0f,
						.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
					}),
				},

				// Swing
				{
					2,
					createRigidBody({
						.position = {0.0_m, 1.0_m},
						.rotation = 0.0_rad,
						.scale = {12.0f, 0.25f},
						.mass = 100.0_kg,
						.friction = 0.2f,
						.restitution = 0.0f,
						.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
					}),
				},

				// Left weight 1
				{
					3,
					createRigidBody({
						.position = {-5.0_m, 2.0_m},
						.rotation = 0.0_rad,
						.scale = {0.5f, 0.5f},
						.mass = 25.0_kg,
						.friction = 0.2f,
						.restitution = 0.0f,
						.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
					}),
				},

				// Left weight 2
				{
					4,
					createRigidBody({
						.position = {-5.5_m, 2.0_m},
						.rotation = 0.0_rad,
						.scale = {0.5f, 0.5f},
						.mass = 25.0_kg,
						.friction = 0.2f,
						.restitution = 0.0f,
						.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
					}),
				},

				// Right weight
				{
					5,
					createRigidBody({
						.position = {5.5_m, 15.0_m},
						.rotation = 0.0_rad,
						.scale = {1.0f, 1.0f},
						.mass = 100.0_kg,
						.friction = 0.2f,
						.restitution = 0.0f,
						.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
					}),
				},
			},
		};
	}
}