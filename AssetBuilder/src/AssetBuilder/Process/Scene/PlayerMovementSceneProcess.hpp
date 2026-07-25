// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"
#include "../../../Subprocess/RigidBody.hpp"

namespace Trident {
	class PlayerMovementSceneProcess final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "PlayerMovement";
			}

			Scene3DData createScene(AssetBuilderLibrary* assetLibrary) const final override {
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

		public:
			explicit PlayerMovementSceneProcess(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}