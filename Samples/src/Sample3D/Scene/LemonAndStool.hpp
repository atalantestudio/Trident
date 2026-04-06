// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/3D/Geometry3D.hpp"
#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/API/Material/Shader.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident::LemonAndStool {
	void initializeScene(Scene3D& scene, AssetLibrary* assetLibrary, GraphicsAPI graphicsApi) {
		const ref<GeometryAsset> lemonGeometryAsset = assetLibrary->getAsset<GeometryAsset>("lemon_1k");
		const ref<GeometryAsset> foldingWoodenStoolGeometryAsset = assetLibrary->getAsset<GeometryAsset>("folding_wooden_stool");

		const ref<Geometry3D> lemonGeometry = createRef<Geometry3D>(lemonGeometryAsset);
		const ref<Geometry3D> foldingWoodenStoolGeometry = createRef<Geometry3D>(foldingWoodenStoolGeometryAsset);

		const uint32 lemonGeometryIndex = scene.registerGeometry(lemonGeometry);
		const uint32 foldingWoodenStoolGeometryIndex = scene.registerGeometry(foldingWoodenStoolGeometry);

		Shader pbrMaterialShader = {
			.descriptorSetLayoutBindings = {
				// Ambient occlusion, roughness, metallic
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::COMPUTE,
				},

				// Diffuse
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::COMPUTE,
				},

				// Normal
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::COMPUTE,
				},
			},
		};

		switch (graphicsApi) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				pbrMaterialShader.modulePath = "Material/Test";

				break;
			}
			case GraphicsAPI::GL: {
				pbrMaterialShader.modulePath = "Material/TestGL";

				break;
			}
		}

		const uint32 pbrMaterialShaderIndex = scene.registerMaterialShader(pbrMaterialShader);

		const uint32 lemonMaterialIndex = scene.registerMaterial({
			.shaderIndex = pbrMaterialShaderIndex,
			.writeDescriptorSetBindings = {
				{
					.descriptorImages = {
						{
							// TODO: lemon_1k needs the PBRSeparatedAORoughness, or I must manually create the ARM map.
							.assetPath = "lemon_ao_1k",
						},
					},
				},
				{
					.descriptorImages = {
						{
							.assetPath = "lemon_diff_1k",
						},
					},
				},
				{
					.descriptorImages = {
						{
							.assetPath = "lemon_nor_gl_1k",
						},
					},
				},
			},
		});

		const uint32 foldingWoodenStoolMaterialIndex = scene.registerMaterial({
			.shaderIndex = pbrMaterialShaderIndex,
			.writeDescriptorSetBindings = {
				{
					.descriptorImages = {
						{
							.assetPath = "folding_wooden_stool_arm_1k",
						},
					},
				},
				{
					.descriptorImages = {
						{
							.assetPath = "folding_wooden_stool_diff_1k",
						},
					},
				},
				{
					.descriptorImages = {
						{
							.assetPath = "folding_wooden_stool_nor_gl_1k",
						},
					},
				},
			},
		});

		scene.registerInstance({
			.position = {0.0_m, -0.5_m, 1.0_m},
			.rotation = {pi<float32>() * 0.5f, 0.3_rad, 0.0_rad},
			.scale = {1.0f, 1.0f, 1.0f},
			.geometryIndex = foldingWoodenStoolGeometryIndex,
			.materialIndex = foldingWoodenStoolMaterialIndex,
		});
		scene.registerInstance({
			.position = {-0.04_m, -0.025_m, 1.0_m},
			.rotation = {0.2_rad, 0.0_rad, 0.0_rad},
			.scale = {1.0f, 1.0f, 1.0f},
			.geometryIndex = lemonGeometryIndex,
			.materialIndex = lemonMaterialIndex,
		});
		scene.registerInstance({
			.position = {0.03_m, -0.025_m, 0.9_m},
			.rotation = {-0.2_rad, 0.5_rad, 0.0_rad},
			.scale = {1.0f, 1.0f, 1.0f},
			.geometryIndex = lemonGeometryIndex,
			.materialIndex = lemonMaterialIndex,
		});
	}
}