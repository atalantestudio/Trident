// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	void ForwardRenderer::updateGeometryDescriptorSets() const {
		const ref<Device> device = getDevice();

		const BaseBufferView descriptorBufferViews[] = {
			cameraBuffer->createView<ForwardCamera>(0, cameraBuffer->getSize()),
			pointLightBuffer->createView<PointLight>(0, pointLightBuffer->getSize()),
			directionalLightBuffer->createView<DirectionalLight>(0, directionalLightBuffer->getSize()),
		};

		const DescriptorImage descriptorImages[] = {
			{
				.imageView = lightmapAtlasImageView,
				.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
				.sampler = lightmapAtlasSampler,
			},
			{
				.imageView = directionalShadowMapImageView,
				.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
				.sampler = directionalShadowMapSampler,
			},
		};

		const WriteDescriptorSet writeDescriptorSets[] = {
			{
				.descriptorSet = dynamicDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = descriptorBufferViews[0],
			},
			{
				.descriptorSet = dynamicDescriptorSet,
				.bindingIndex = 1,
				.descriptorBufferViews = descriptorBufferViews[1],
			},
			{
				.descriptorSet = directionalShadowMapDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = descriptorBufferViews[2],
			},
			{
				.descriptorSet = staticGeometryDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = descriptorBufferViews[0],
			},
			{
				.descriptorSet = staticGeometryDescriptorSet,
				.bindingIndex = 2,
				.descriptorBufferViews = descriptorBufferViews[2],
			},
			{
				.descriptorSet = staticGeometryDescriptorSet,
				.bindingIndex = 3,
				.descriptorImages = descriptorImages[1],
			},
		};

		device->updateDescriptorSets(writeDescriptorSets);

		if (scene->getAsset()->lightmapAtlasImageAsset != nullptr) {
			const WriteDescriptorSet writeDescriptorSets[] = {
				{
					.descriptorSet = staticGeometryDescriptorSet,
					.bindingIndex = 1,
					.descriptorImages = descriptorImages[0],
				},
			};

			device->updateDescriptorSets(writeDescriptorSets);
		}
	}
}