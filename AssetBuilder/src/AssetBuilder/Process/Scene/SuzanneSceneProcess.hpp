// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/MeshSceneBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"
#include "../../../Subprocess/RigidBody.hpp"

namespace Trident {
	class SuzanneSceneProcess final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "Suzanne";
			}

			Scene3DData createScene(AssetBuilderLibrary* assetLibrary) const final override {
				MeshSceneBuilder builder;
				builder.addMaterialShader("Lightmapped.frag", ShaderStage::FRAGMENT, "main", {
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
				});

				const uint32 materialIndex = builder.addMaterial(assetLibrary->getAsset<MaterialAsset>("lightmapped_white"));

				const ref<GeometryAsset> geometryAsset = assetLibrary->getAsset<GeometryAsset>("suzanne");

				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({128.0f, 128.0f}),
						.textureFiltering = {
							.translation = {0.0f, 0.0f},
							.rotation = 0.0f,
							.scale = {1.0f, 1.0f},
						},
						.materialIndex = materialIndex,
						.luxelScale = {4.0f, 4.0f},
						.carvings = {},
					},
					{
						.translation = {0.0f, 0.0f, 0.0f},
						.axis = {1.0f, 0.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				builder.placeMesh({
					.asset = geometryAsset,
					.transform = {
						.translation = {0.0f, 38.5f, 0.0f},

						// The model uses Z as up and is very small in scale.
						.eulerAngles = {radians(-90.0_deg), radians(180.0_deg), 0.0_rad},
						.scale = {256.0f, 256.0f, 256.0f},
					},
					.materialIndex = materialIndex,
					.lightmapScale = {512.0f, 512.0f},
					.luxelScale = {1.0f, 1.0f},
				});

				return {
					.staticScene = builder.build(),
					.entityCount = 1,
					.geometryAssetIdentifiers = {
						38,
					},
					.geometryMappings = {
						{0, {0}},
					},
					.materialMappings = {
						{0, {1}},
					},
					.rigidBodyMappings = {
						// Player
						{
							0,
							createRigidBody({
								.position = {0.0f, 38.5f, -64.0f},
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
			explicit SuzanneSceneProcess(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}