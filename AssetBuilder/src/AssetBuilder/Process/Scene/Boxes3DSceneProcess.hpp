// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"
#include "../../../Subprocess/RigidBody.hpp"

namespace Trident {
	class Boxes3DSceneProcess final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "Boxes3D";
			}

			Scene3DData createScene(AssetBuilderLibrary* assetLibrary) const final override {
				return {
					.staticScene = {},
					.entityCount = 2,
					.geometryAssetIdentifiers = {
						38,
						40,
					},
					.geometryMappings = {
						{0, {1}},
						{1, {0}},
					},
					.materialMappings = {
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

		public:
			explicit Boxes3DSceneProcess(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}