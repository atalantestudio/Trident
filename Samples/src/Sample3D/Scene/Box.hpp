// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/3D/BoxGeometry3D.hpp"
#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/API/Material/Shader.hpp"
#include "Engine/API/Scene/Scene3D.hpp"
#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident::Box {
	void initializeScene(Scene3D& scene, AssetLibrary* assetLibrary, GraphicsAPI graphicsApi) {
		const ref<Geometry3D> geometry = createRef<BoxGeometry3D>(vec3<float32>(1.0f, 1.0f, 1.0f));
		const uint32 geometryIndex = scene.registerGeometry(geometry);

		Shader testMaterialShader = {
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
				testMaterialShader.modulePath = "Material/Test";

				break;
			}
			case GraphicsAPI::GL: {
				testMaterialShader.modulePath = "Material/TestGL";

				break;
			}
		}

		const uint32 testMaterialShaderIndex = scene.registerMaterialShader(testMaterialShader);

		const uint32 testMaterialIndex = scene.registerMaterial({
			.shaderIndex = testMaterialShaderIndex,
			.writeDescriptorSetBindings = {
				{
					.descriptorImages = {
						{
							.assetPath = "red",
						},
					},
				},
				{
					.descriptorImages = {
						{
							.assetPath = "green",
						},
					},
				},
				{
					.descriptorImages = {
						{
							.assetPath = "blue",
						},
					},
				},
			},
		});

		scene.registerInstance({
			.position = {0.0_m, 0.0_m, 3.0_m},
			.rotation = {0.0_rad, 0.0_rad, 0.0_rad},
			.scale = {1.0f, 1.0f, 1.0f},
			.geometryIndex = geometryIndex,
			.materialIndex = testMaterialIndex,
		});
	}
}