// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "RigidBody.hpp"

namespace Trident {
	Scene3DData createBoxScene() {
		return {
			.staticVertexCount = 0,
			.staticIndexCount = 0,
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
				// Floor
				/* {
					0,
					createRigidBody({
						.position = {0.0_m, -0.05_m, 0.0_m},
						.rotation = {0.0_rad, 0.0_rad, 0.15_rad},
						.scale = {10.0f, 0.1f, 10.0f},
						.mass = max<float32>(),
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				}, */

				// Box 1
				{
					0,
					createRigidBody({
						.position = {0.0_m, 0.0_m, 2.0_m},
						.rotation = {0.0_rad, pi<float32>() * 0.25f, 0.0_rad},
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
						.position = {0.0_m, 2.5_m, 2.0_m},
						.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
						.scale = {1.0f, 1.0f, 1.0f},
						.mass = 25.0_kg,
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				},
			},
		};
	}

	Scene3DData createLemonsScene() {
		return {
			.staticVertexCount = 0,
			.staticIndexCount = 0,
			.entityCount = 2,
			.geometryAssetIdentifiers = {
				// TODO(debug-only): lemon_1k
				40,
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
						.position = {0.3_m, 0.0_m, 1.25_m},
						.rotation = {-pi<float32>() * 0.5f, -0.2_rad, pi<float32>()},
						.scale = {10.0f, 10.0f, 10.0f},
						.mass = max<float32>(),
						.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
						.friction = 0.2f,
						.restitution = 0.0f,
					}),
				},
				{
					1,
					createRigidBody({
						.position = {-0.35_m, -0.2_m, 1.25_m},
						.rotation = {-0.3_rad, -0.3_rad, pi<float32>() * 0.5f},
						.scale = {10.0f, 10.0f, 10.0f},
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