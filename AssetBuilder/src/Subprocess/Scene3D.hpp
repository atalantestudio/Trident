// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "RigidBody3DComponent.hpp"

namespace Trident {
	Scene3DData createLemonsScene() {
		return {
			.staticVertexCount = 0,
			.staticIndexCount = 0,
			.entityCount = 2,
			.geometryAssetIdentifiers = {
				// TODO(debug-only): lemon_1k
				41,
			},
			.geometryMappings = {
				{0, {0}},
				{1, {0}},
			},
			.rigidBodyMappings = {
				{
					0,
					createRigidBody({
						.position = {0.3_m, 0.0_m, 1.25_m},
						.rotation = {-pi<float32>() * 0.5f, -0.2_rad, pi<float32>()},
						.scale = {10.0f, 10.0f, 10.0f},
					}),
				},
				{
					1,
					createRigidBody({
						.position = {-0.35_m, -0.2_m, 1.25_m},
						.rotation = {-0.3_rad, -0.3_rad, pi<float32>() * 0.5f},
						.scale = {10.0f, 10.0f, 10.0f},
					}),
				},
			},
		};
	}
}