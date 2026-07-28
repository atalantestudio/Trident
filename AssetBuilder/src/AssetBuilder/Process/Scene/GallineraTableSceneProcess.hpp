// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Builder/MeshSceneBuilder.hpp"
#include "AssetBuilder/Process/Scene/Scene3DProcess.hpp"
#include "../../../Subprocess/RigidBody.hpp"

namespace Trident {
	class GallineraTableSceneProcess final : public Scene3DProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "gallinera_table";
			}

			Scene3DData createScene(AssetBuilderLibrary* assetLibrary) const final override {
				MeshSceneBuilder builder({
					.skyColor = {0.6f, 0.7f, 0.9f},
					.sunDirection = {1.0f, 1.0f, 1.0f},
					.sunColor = {1.0f, 1.0f, 1.0f},
					.sunIntensity = 1.0f,
				});
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

				const uint32 materialIndex = builder.addMaterial(assetLibrary->getAsset<MaterialAsset>("white"));

				const ref<GeometryAsset> geometryAsset = assetLibrary->getAsset<GeometryAsset>("gallinera_table_1k");

				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({128.0f, 128.0f}),
						.textureFiltering = {
							.translation = {0.0f, 0.0f},
							.rotation = 0.0f,
							.scale = {1.0f, 1.0f},
						},
						.materialIndex = materialIndex,
						.lightmap = {
							.luxelScale = {4.0f, 4.0f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
						.carvings = {},
					},
					{
						.translation = {0.0f, 0.0f, 0.0f},
						.axis = {1.0f, 0.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({128.0f, 64.0f}),
						.textureFiltering = {
							.translation = {0.0f, 0.0f},
							.rotation = 0.0f,
							.scale = {1.0f, 1.0f},
						},
						.materialIndex = materialIndex,
						.lightmap = {
							.luxelScale = {4.0f, 4.0f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
						.carvings = {},
					},
					{
						.translation = {0.0f, 32.0f, 64.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = 0.0_rad,
					}
				);

				builder.placeSurface(
					{
						.geometry = builder.createWallGeometry({128.0f, 64.0f}),
						.textureFiltering = {
							.translation = {0.0f, 0.0f},
							.rotation = 0.0f,
							.scale = {1.0f, 1.0f},
						},
						.materialIndex = materialIndex,
						.lightmap = {
							.luxelScale = {4.0f, 4.0f},
							.tintColor = {1.0f, 1.0f, 1.0f},
						},
						.carvings = {},
					},
					{
						.translation = {64.0f, 32.0f, 0.0f},
						.axis = {0.0f, 1.0f, 0.0f},
						.angle = radians(-90.0_deg),
					}
				);

				builder.placeMesh({
					.asset = geometryAsset,
					.transform = {
						.translation = {0.0f, 0.0f, 0.0f},
						.eulerAngles = {radians(90.0_deg), 0.0_rad, 0.0_rad},
						.scale = {64.0f, 64.0f, 64.0f},
					},
					.materialIndex = materialIndex,
					.lightmap = {
						.luxelScale = {1.0f, 1.0f},
						.tintColor = {1.0f, 1.0f, 1.0f},
					},
					.lightmapScale = {512.0f, 512.0f},
				});

				return {
					.staticScene = builder.build(),
					.entityCount = 1,
					.geometryAssetIdentifiers = {
						38,
					},
					.materialShaders = {
						{
							.name = "PBR.frag",
							.stage = ShaderStage::FRAGMENT,
							.entryPoint = "main",
							.descriptorSetLayoutBindings = {
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
								{
									.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
									.descriptorCount = 1,
									.shaderStages = ShaderStage::FRAGMENT,
								},
							},
							.materialAssetRange = {0, 0},
						},
					},
					.materialAssets = {
						assetLibrary->getAsset<MaterialAsset>("gallinera_table_1k"),
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
								.position = {0.0f, 32.0f, -128.0f},
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
			explicit GallineraTableSceneProcess(AssetBuilderLibrary* assetLibrary) :
				Scene3DProcess(assetLibrary)
			{}
	};
}