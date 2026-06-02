// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "RigidBody.hpp"

namespace Trident {
	Scene3DData createBoxes3DScene(std::filesystem::path rootOutputPath) {
		return {
			.staticScene = {},
			.entityCount = 2,
			.geometryAssetIdentifiers = {
				// TODO(debug-only): UnitBox
				38,

				// TODO(debug): RectangularCuboid
				40,
			},
			.geometryMappings = {
				{0, {1}},
				{1, {0}},
			},
			.materialMappings = {
				// NOTE: Counting default material for visibility rendering.
				{0, {1}},
				{1, {1}},
			},
			.rigidBodyMappings = {
				// Floor
				{
					0,
					createRigidBody({
						.position = {0.0f, 0.0f, 0.2f},
						.rotation = {0.0f, 0.0f, 0.15f},
						.scale = {5.0f, 0.1f, 1.0f},
						.mass = max<float32>(),
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.0f,
						.restitution = 0.0f,
					}),
				},

				// Box 1
				/* {
					0,
					createRigidBody({
						.position = {0.0_m, 0.0_m, 0.0_m},
						// .rotation = {0.0_rad, pi<float32>() * 0.25f, 0.0_rad},
						.rotation = {0.0f, 0.0f, 0.0f},
						.scale = {1.0f, 1.0f, 1.0f},
						.mass = max<float32>(),
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				}, */

				// Box 2
				{
					1,
					createRigidBody({
						.position = {0.0_m, 2.2_m, 0.0_m},
						// .rotation = {0.0f, 0.0f, 0.0f},
						.rotation = {0.0f, pi<float32>() * 0.25f, pi<float32>() * 0.25f},
						.scale = {1.0f, 1.0f, 1.0f},
						.mass = 3.0_kg,
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.0f,
						.restitution = 0.0f,
					}),
				},
			},
		};
	}

	Scene3DData createEdgeContactScene(std::filesystem::path rootOutputPath) {
		return {
			.staticScene = {},
			.entityCount = 2,
			.geometryAssetIdentifiers = {
				// TODO(debug-only): UnitBox
				38,
			},
			.geometryMappings = {
				{0, {0}},
				{1, {0}},
			},
			.materialMappings = {
				// NOTE: Counting default material for visibility rendering.
				{0, {1}},
				{1, {1}},
			},
			.rigidBodyMappings = {
				// Box 1
				{
					0,
					createRigidBody({
						.position = {0.0_m, 0.0_m, 0.0_m},
						.rotation = {0.0f, 0.0f, 0.0f},
						.scale = {1.0f, 1.0f, 1.0f},
						.mass = max<float32>(),
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				},

				// Box 2
				{
					1,
					createRigidBody({
						.position = {1.0_m, 2.0_m, 0.0_m},
						.rotation = {0.0f, -0.3f, /*-pi<float32>() / 4.0f */-0.4f},
						.scale = {1.0f, 1.0f, 1.0f},
						.mass = 3.0_kg,
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.0f,
						.restitution = 0.0f,
					}),
				},
			},
		};
	}

	Scene3DData createBoxStackingScene(std::filesystem::path rootOutputPath) {
		return {
			.staticScene = {},
			.entityCount = 4,
			.geometryAssetIdentifiers = {
				// TODO(debug-only): RectangularCuboid
				40,
			},
			.geometryMappings = {
				{0, {0}},
				{1, {0}},
				{2, {0}},
				{3, {0}},
			},
			.materialMappings = {
				// NOTE: Counting default material for visibility rendering.
				{0, {1}},
				{1, {1}},
				{2, {1}},
				{3, {1}},
			},
			.rigidBodyMappings = {
				// Floor
				{
					0,
					createRigidBody({
						.position = {0.0f, -0.05f, 0.0f},
						.rotation = {0.0f, 0.0f, 0.0f},
						.scale = {10.0f, 0.1f, 10.0f},
						.mass = max<float32>(),
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				},

				// Box 1
				{
					1,
					createRigidBody({
						.position = {0.0_m, 1.5_m, 0.0_m},
						.rotation = {0.2f, 0.1f, 0.0f},
						.scale = {1.0f, 1.0f, 1.0f},
						.mass = 3.0_kg,
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				},

				// Box 2
				{
					2,
					createRigidBody({
						.position = {0.1_m, 3.0_m, -0.1_m},
						.rotation = {0.0f, 0.0f, 0.0f},
						.scale = {1.0f, 1.0f, 1.0f},
						.mass = 3.0_kg,
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				},

				// Box 3
				{
					3,
					createRigidBody({
						.position = {0.0_m, 4.5_m, 0.05_m},
						.rotation = {0.0f, 0.0f, 0.0f},
						// .rotation = {0.0f, pi<float32>() * 0.25f, pi<float32>() * 0.25f},
						.scale = {1.0f, 1.0f, 1.0f},
						.mass = 3.0_kg,
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				},
			},
		};
	}

	Scene3DData createLemonsScene(std::filesystem::path rootOutputPath) {
		return {
			.staticScene = {},
			.entityCount = 2,
			.geometryAssetIdentifiers = {
				// TODO(debug-only): lemon_1k
				41,
			},
			.geometryMappings = {
				{0, {0}},
				{1, {0}},
			},
			.materialMappings = {
				// NOTE: Counting default material for visibility rendering.
				{0, {1}},
				{1, {1}},
			},
			.rigidBodyMappings = {
				{
					0,
					createRigidBody({
						.position = svec3<float32>(vec3<float32>{0.3_m, 0.0_m, 1.25_m} / 10.0f),
						.rotation = {pi<float32>() * 0.5f, -0.2_rad, pi<float32>()},
						.scale = {1.0f, 1.0f, 1.0f},
						.mass = max<float32>(),
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				},
				{
					1,
					createRigidBody({
						.position = svec3<float32>(vec3<float32>{-0.35_m, -0.2_m, 1.25_m} / 10.0f),
						.rotation = {-0.3_rad, -0.3_rad, pi<float32>() * 0.5f},
						.scale = {1.0f, 1.0f, 1.0f},
						.mass = max<float32>(),
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				},
			},
		};
	}

	Scene3DData createPlayerMovementScene(std::filesystem::path rootOutputPath) {
		return {
			.staticScene = {},
			.entityCount = 3,
			.geometryAssetIdentifiers = {
				// TODO(debug-only): UnitBox
				38,
			},
			.geometryMappings = {
				{0, {0}},
				{1, {0}},
				{2, {0}},
			},
			.materialMappings = {
				// NOTE: Counting default material for visibility rendering.
				{0, {1}},
				{1, {1}},
				{2, {1}},
			},
			.rigidBodyMappings = {
				/* // Player
				{
					2,
					createRigidBody({
						.position = {0.0_m, 2.0_m, 0.0_m},
						.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
						.scale = {0.7f, 1.8f, 0.7f},
						.mass = 70.0_kg,
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				},

				// Floor
				{
					1,
					createRigidBody({
						.position = {0.0_m, -0.05_m, 0.0_m},
						.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
						.scale = {10.0f, 0.1f, 10.0f},
						.mass = max<float32>(),
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				},

				// Box
				{
					2,
					createRigidBody({
						.position = {1.0_m, 0.5_m, 3.0_m},
						.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
						.scale = {1.0f, 1.0f, 1.0f},
						.mass = max<float32>(),
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				}, */

				// Player
				{
					0,
					createRigidBody({
						.position = {0.0f, 128.0f, 0.0f},
						.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
						.scale = {33.0f, 77.0f, 33.0f},
						.mass = 70.0_kg,
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				},

				// Floor
				{
					1,
					createRigidBody({
						.position = {0.0f, -8.0f, 0.0f},
						.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
						.scale = {512.0f, 16.0f, 512.0f},
						.mass = max<float32>(),
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				},

				// Box
				{
					2,
					createRigidBody({
						.position = {128.0f, 8.0f, 128.0f},
						.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
						.scale = {64.0f, 16.0f, 64.0f},
						// .mass = 20.0_kg,
						.mass = max<float32>(),
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				},
			},
		};
	}
}