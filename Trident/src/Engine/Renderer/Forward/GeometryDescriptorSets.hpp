// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	void ForwardRenderer::updateGeometryDescriptorSets() const {
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
				.bindingIndex = 1,
				.descriptorImages = descriptorImages[0],
			},
		};

		uint32 writeDescriptorSetCount = 4;

		if (scene->getAsset()->lightmapAtlasImageAsset != nullptr) {
			writeDescriptorSetCount = 5;
		}

		getDevice()->updateDescriptorSets(view<WriteDescriptorSet>(writeDescriptorSets, writeDescriptorSetCount));
	}
}