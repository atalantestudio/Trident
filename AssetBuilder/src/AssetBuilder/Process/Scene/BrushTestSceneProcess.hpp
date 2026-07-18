// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/StaticSceneBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"
#include "../../../Subprocess/RigidBody.hpp"

namespace Trident {
	class BrushTestSceneProcess final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "BrushTest";
			}

			Scene3DData createScene(std::filesystem::path rootOutputPath) const final override {
				StaticSceneBuilder builder(rootOutputPath);
				builder.addMaterialShader(
					"Lightmapped.frag",
					ShaderStage::FRAGMENT,
					"main",
					{
						{
							.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
							.descriptorCount = 1,
							.shaderStages = ShaderStage::FRAGMENT,
						},
						{
							.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
							.descriptorCount = 1,
							.shaderStages = ShaderStage::FRAGMENT,
						},
					}
				);

				builder.addMaterial(100, 0);

				builder.addSurface({
					.materialIndex = 0,
					.transform = {
						.position = {56.0f, 0.0f, 84.0f},
						.rotation = {0.0f, 0.0f, 0.0f},
						.scale = {144.0f, 200.0f},
					},
					.textureFiltering = {
						.translation = {0.25f, 0.625f},
						.rotation = 0.0f,
						.scale = {(1.0f / 256.0f) / -0.25f, (1.0f / 256.0f) / 0.25f},
					},
					.lightmapScale = {4.0f, 4.0f},
				});

				return {
					.staticScene = builder.build(),
					.entityCount = 1,
					.geometryAssetIdentifiers = {
						// TODO(debug-only): UnitBox
						38,
					},
					.geometryMappings = {
						{0, {0}},
					},
					.materialMappings = {
						// NOTE: Counting default material for visibility rendering.
						{0, {1}},
					},
					.rigidBodyMappings = {
						// Player
						{
							0,
							createRigidBody({
								.position = {0.0f, 38.5f, 0.0f},
								.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
								.scale = {33.0f, 77.0f, 33.0f},
								.uniformScale = 33.0f,
								.mass = 70.0_kg,
								.momentOfInertia = MomentOfInertia3D::SOLID_CUBOID,
								.friction = 0.2f,
								.restitution = 0.0f,
							}),
						},
					},
				};
			}

		public:
			explicit BrushTestSceneProcess(BuildAssetMap* assetMap) :
				Scene3DProcess(assetMap)
			{}
	};
}