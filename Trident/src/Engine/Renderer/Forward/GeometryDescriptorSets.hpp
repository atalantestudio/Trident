// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	void ForwardRenderer::updateGeometryDescriptorSets() const {
		const ref<BaseBufferView> descriptorBufferViews[1] = {
			cameraBuffer->createView<ForwardCamera>(0, cameraBuffer->getSize()),
		};

		const DescriptorImage descriptorImages[1] = {
			{
				.imageView = lightmapAtlasImageView,
				.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
				.sampler = lightmapAtlasSampler,
			},
		};

		const WriteDescriptorSet writeDescriptorSets[3] = {
			{
				.descriptorSet = forwardDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = descriptorBufferViews[0],
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

		uint32 writeDescriptorSetCount = 2;

		if (scene->getAsset()->lightmapAtlasImageAsset != nullptr) {
			writeDescriptorSetCount = 3;
		}

		getDevice()->updateDescriptorSets(view<WriteDescriptorSet>(writeDescriptorSets, writeDescriptorSetCount));
	}
}