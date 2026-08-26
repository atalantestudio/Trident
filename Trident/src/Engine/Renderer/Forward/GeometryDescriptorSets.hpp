// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	void ForwardRenderer::updateGeometryDescriptorSets() const {
		const ref<Device> device = getDevice();

		const BaseBufferView descriptorBufferViews[] = {
			cameraBuffer->createView<ForwardCamera>(0, cameraBuffer->getSize()),
			directionalLightBuffer->createView<DirectionalLight>(0, directionalLightBuffer->getSize()),
			pointLightBuffer->createView<PointLight>(0, pointLightBuffer->getSize()),
		};

		const DescriptorImage descriptorImages[] = {
			{
				.imageView = skyBoxImageView,
				.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,

				// TODO(forward/environment-map): Use a different sampler?
				.sampler = directionalShadowMapSampler,
			},
			{
				.imageView = directionalShadowMapImageView,
				.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
				.sampler = directionalShadowMapSampler,
			},
		};

		const WriteDescriptorSet writeDescriptorSets[] = {
			// Camera
			{
				.descriptorSet = skyBoxDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = descriptorBufferViews[0],
			},

			// Skybox
			{
				.descriptorSet = skyBoxDescriptorSet,
				.bindingIndex = 1,
				.descriptorImages = descriptorImages[0],
			},

			// Camera
			{
				.descriptorSet = sceneDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = descriptorBufferViews[0],
			},

			// Directional light
			{
				.descriptorSet = sceneDescriptorSet,
				.bindingIndex = 1,
				.descriptorBufferViews = descriptorBufferViews[1],
			},

			// Point light
			{
				.descriptorSet = sceneDescriptorSet,
				.bindingIndex = 2,
				.descriptorBufferViews = descriptorBufferViews[2],
			},

			// Directional shadow map
			{
				.descriptorSet = sceneDescriptorSet,
				.bindingIndex = 3,
				.descriptorImages = descriptorImages[1],
			},

			// Point light
			{
				.descriptorSet = directionalShadowMapDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = descriptorBufferViews[1],
			},
		};

		device->updateDescriptorSets(writeDescriptorSets);

		if (scene->getAsset()->lightmapAtlasImageAsset != nullptr) {
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