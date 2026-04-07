// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "RigidBody2DComponent.hpp"

namespace Trident {
	Scene2DData createBoxesScene() {
		return {
			.staticVertexCount = 0,
			.staticIndexCount = 0,
			.entityCount = 2,
			.geometryAssetIdentifiers = {
				// TODO(debug-only): unit_square
				484634187,
			},
			.geometryMappings = {
				{0, {0}},
				{1, {0}},
			},
			.rigidBodyMappings = {
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
}