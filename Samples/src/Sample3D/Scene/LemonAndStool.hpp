// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Material/Shader.hpp"
#include "Engine/API/Scene/Scene3D.hpp"

namespace Trident {
	Scene3D createLemonsAndStoolScene(AssetLibrary* assetLibrary, GraphicsAPI graphicsApi) {
		Scene3D scene;

		const ref<GeometryAsset> lemonGeometryAsset = assetLibrary->getAsset<GeometryAsset>("lemon_1k");
		const ref<GeometryAsset> foldingWoodenStoolGeometryAsset = assetLibrary->getAsset<GeometryAsset>("folding_wooden_stool");

		const ref<Geometry3D> lemonGeometry = createRef<Geometry3D>(lemonGeometryAsset);
		const ref<Geometry3D> foldingWoodenStoolGeometry = createRef<Geometry3D>(foldingWoodenStoolGeometryAsset);

		const uint32 lemonGeometryIndex = scene.registerGeometry(lemonGeometry);
		const uint32 foldingWoodenStoolGeometryIndex = scene.registerGeometry(foldingWoodenStoolGeometry);

		const uint32 defaultMaterialShaderIndex = scene.registerMaterialShader({
			.modulePath = "Visibility/Material/Default",
			.descriptorSetLayoutBindings = {},
		});

		const uint32 defaultMaterialIndex = scene.registerMaterial({
			.shaderIndex = defaultMaterialShaderIndex,
			.writeDescriptorSetBindings = {},
		});

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
				pbrMaterialShader.modulePath = "Visibility/Material/Test";

				break;
			}
			case GraphicsAPI::GL: {
				pbrMaterialShader.modulePath = "Visibility/Material/TestGL";

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
							.assetPath = "lemon_arm_1k",
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
							.assetPath = "lemon_arm_1k",
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

		// Lemon 1
		scene.registerInstance({
			.position = {-0.1_m, 0.0_m, 0.3_m},
			.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
			.scale = {1.0f, 1.0f, 1.0f},
			.geometryIndex = lemonGeometryIndex,
			.materialIndex = lemonMaterialIndex,
		});

		// Lemon 2
		scene.registerInstance({
			.position = {0.0_m, 0.0_m, 0.3_m},
			.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
			.scale = {1.0f, 1.0f, 1.0f},
			.geometryIndex = lemonGeometryIndex,
			.materialIndex = lemonMaterialIndex,
		});

		// Folding wooden stool
		scene.registerInstance({
			.position = {0.5_m, -0.225_m, 1.0_m},
			.rotation = {pi<float32>() * 0.5f, 0.3_rad, 0.0_rad},
			.scale = {1.0f, 1.0f, 1.0f},
			.geometryIndex = foldingWoodenStoolGeometryIndex,
			.materialIndex = foldingWoodenStoolMaterialIndex,
		});

		return scene;
	}
}