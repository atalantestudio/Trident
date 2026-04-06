// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/3D/Geometry3D.hpp"
#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/API/Material/Shader.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident::Lemons {
	void initializeScene(Scene3D& scene, AssetLibrary* assetLibrary, GraphicsAPI graphicsApi) {
		const ref<GeometryAsset> lemonGeometryAsset = assetLibrary->getAsset<GeometryAsset>("lemon_1k");
		const ref<Geometry3D> lemonGeometry = createRef<Geometry3D>(lemonGeometryAsset);
		const uint32 lemonGeometryIndex = scene.registerGeometry(lemonGeometry);

		Shader pbrMaterialShader = {
			.descriptorSetLayoutBindings = {
				// Ambient occlusion
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::FRAGMENT,
				},

				// Roughness
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::FRAGMENT,
				},

				// Diffuse
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::FRAGMENT,
				},

				// Normal
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::FRAGMENT,
				},
			},
		};

		switch (graphicsApi) {
			case GraphicsAPI::DX12:
			case GraphicsAPI::VK: {
				pbrMaterialShader.modulePath = "Material/PBRSeparatedAORoughness.frag";

				break;
			}
			case GraphicsAPI::GL: {
				pbrMaterialShader.modulePath = "Material/PBRSeparatedAORoughnessSingleSet.frag";

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
							.assetPath = "lemon_ao_1k",
						},
					},
				},
				{
					.descriptorImages = {
						{
							.assetPath = "lemon_rough_1k",
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

		scene.registerInstance({
			.position = {0.3_m, 0.0_m, 1.25_m},
			.rotation = {-pi<float32>() * 0.5f, -0.2_rad, pi<float32>()},
			.scale = {10.0f, 10.0f, 10.0f},
			.geometryIndex = lemonGeometryIndex,
			.materialIndex = lemonMaterialIndex,
		});
		scene.registerInstance({
			.position = {-0.35_m, -0.2_m, 1.25_m},
			.rotation = {-0.3_rad, -0.3_rad, pi<float32>() * 0.5f},
			.scale = {10.0f, 10.0f, 10.0f},
			.geometryIndex = lemonGeometryIndex,
			.materialIndex = lemonMaterialIndex,
		});
	}
}