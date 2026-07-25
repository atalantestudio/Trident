// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"
#include "../../../Subprocess/RigidBody.hpp"

namespace Trident {
	class LemonsSceneProcess final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "Lemons";
			}

			Scene3DData createScene(AssetBuilderLibrary* assetLibrary) const final override {
				return {
					.staticScene = {},
					.entityCount = 2,
					.geometryAssetIdentifiers = {
						// TODO(debug-only): lemon_1k
						42,
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

		public:
			explicit LemonsSceneProcess(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}