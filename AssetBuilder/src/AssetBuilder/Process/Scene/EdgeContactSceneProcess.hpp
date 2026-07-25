// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"
#include "../../../Subprocess/RigidBody.hpp"

namespace Trident {
	class EdgeContactSceneProcess final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "EdgeContact";
			}

			Scene3DData createScene(AssetBuilderLibrary* assetLibrary) const final override {
				return {
					.staticScene = {},
					.entityCount = 2,
					.geometryAssetIdentifiers = {
						38,
					},
					.geometryMappings = {
						{0, {0}},
						{1, {0}},
					},
					.materialMappings = {
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

		public:
			explicit EdgeContactSceneProcess (AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}