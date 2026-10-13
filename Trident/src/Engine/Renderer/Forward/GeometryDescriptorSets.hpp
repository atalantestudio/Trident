// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	void ForwardRenderer::updateGeometryDescriptorSets() const {
		const ref<Device> device = getDevice();
		const ref<Scene3DAsset> sceneAsset = scene->getAsset();

		const BaseBufferView descriptorBufferViews[] = {
			cameraBuffer->createView<ForwardCamera>(0, cameraBuffer->getSize()),
			pointLightBuffer->createView<PointLight>(0, pointLightBuffer->getSize()),

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				directionalLightBuffer->createView<DirectionalLight>(0, directionalLightBuffer->getSize()),
				directionalLightBuffer->createView<mat4x4<float32>>(0, sizeof(mat4x4<float32>)),
				directionalLightBuffer->createView<mat4x4<float32>>(sizeof(mat4x4<float32>), sizeof(mat4x4<float32>)),
				directionalLightBuffer->createView<mat4x4<float32>>(sizeof(mat4x4<float32>) * 2, sizeof(mat4x4<float32>)),
			#endif
		};

		const DescriptorImage descriptorImages[] = {
			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				{
					.imageView = directionalShadowMapImageView,
					.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
					.sampler = defaultSampler,
				},
			#endif
			{},
		};

		const WriteDescriptorSet writeDescriptorSets[] = {
			// Camera
			{
				.descriptorSet = sceneDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = descriptorBufferViews[0],
			},

			// Point light
			{
				.descriptorSet = sceneDescriptorSet,
				.bindingIndex = 1,
				.descriptorBufferViews = descriptorBufferViews[1],
			},

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				// Directional light
				{
					.descriptorSet = sceneDescriptorSet,
					.bindingIndex = 2,
					.descriptorBufferViews = descriptorBufferViews[2],
				},

				// Directional shadow map
				{
					.descriptorSet = sceneDescriptorSet,
					.bindingIndex = 3,
					.descriptorImages = descriptorImages[0],
				},
			#endif

			#if defined(FORWARD_ENABLE_DIRECTIONAL_SHADOW_MAP)
				// Point light
				{
					.descriptorSet = directionalShadowMapCascadeDescriptorSets[0],
					.bindingIndex = 0,
					.descriptorBufferViews = descriptorBufferViews[3],
				},
				{
					.descriptorSet = directionalShadowMapCascadeDescriptorSets[1],
					.bindingIndex = 0,
					.descriptorBufferViews = descriptorBufferViews[4],
				},
				{
					.descriptorSet = directionalShadowMapCascadeDescriptorSets[2],
					.bindingIndex = 0,
					.descriptorBufferViews = descriptorBufferViews[5],
				},
			#endif
		};

		device->updateDescriptorSets(writeDescriptorSets);

		if (sceneAsset->skyboxImageAsset != nullptr) {
			const DescriptorImage skyboxDescriptorImage = {
				.imageView = skyboxImageView,
				.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
				.sampler = defaultSampler,
			};

			const WriteDescriptorSet writeDescriptorSets[] = {
				// Camera
				{
					.descriptorSet = skyboxDescriptorSet,
					.bindingIndex = 0,
					.descriptorBufferViews = descriptorBufferViews[0],
				},

				// Skybox
				{
					.descriptorSet = skyboxDescriptorSet,
					.bindingIndex = 1,
					.descriptorImages = skyboxDescriptorImage,
				},
			};

			device->updateDescriptorSets(writeDescriptorSets);
		}

		if (sceneAsset->lightmapAtlasImageAsset != nullptr) {
			const DescriptorImage lightmapAtlasDescriptorImage = {
				.imageView = lightmapAtlasImageView,
				.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
				.sampler = lightmapAtlasSampler,
			};

			const WriteDescriptorSet lightmapAtlasWriteDescriptorSet = {
				.descriptorSet = lightmapAtlasDescriptorSet,
				.bindingIndex = 0,
				.descriptorImages = lightmapAtlasDescriptorImage,
			};

			device->updateDescriptorSets(lightmapAtlasWriteDescriptorSet);
		}
	}
}