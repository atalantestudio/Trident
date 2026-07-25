// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"
#include "../../../Subprocess/RigidBody.hpp"

namespace Trident {
	class BoxStackingSceneProcess final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "BoxStacking";
			}

			Scene3DData createScene(AssetBuilderLibrary* assetLibrary) const final override {
				return {
					.staticScene = {},
					.entityCount = 4,
					.geometryAssetIdentifiers = {
						40,
					},
					.geometryMappings = {
						{0, {0}},
						{1, {0}},
						{2, {0}},
						{3, {0}},
					},
					.materialMappings = {
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

		public:
			explicit BoxStackingSceneProcess(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}