// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene2DProcess.hpp"
#include "../../../Subprocess/RigidBody.hpp"

namespace Trident {
	class BoxesSceneProcess final : public Scene2DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "Boxes";
			}

			Scene2DData createScene(AssetBuilderLibrary* assetLibrary) const final override {
				return {
					.staticScene = {},
					.entityCount = 2,
					.geometryAssetIdentifiers = {
						35,
					},
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
								.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
								.friction = 0.2f,
								.restitution = 0.0f,
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
								.momentOfInertia = MomentOfInertia2D::THIN_RECTANGULAR_PLATE,
								.friction = 0.2f,
								.restitution = 0.0f,
							}),
						},
					},
				};
			}

		public:
			explicit BoxesSceneProcess(AssetBuilderLibrary* assetLibrary) :
				Scene2DProcess(assetLibrary)
			{}
	};
}