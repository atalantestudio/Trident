// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	void ForwardRenderer::updateGeometryDescriptorSets() const {
		const ref<Device> device = getDevice();
		const uint64 alignment = device->getPhysicalDevice()->getLimits().minStorageBufferOffsetAlignment;
		const ref<Scene3DAsset> sceneAsset = scene->getAsset();
		const uint64 staticPositionAlignedRangeSize = ceil(sceneAsset->geometryRanges[1].count, alignment);
		const uint64 staticNormalAlignedRangeSize = ceil(sceneAsset->geometryRanges[2].count, alignment);

		const ref<BaseBufferView> descriptorBufferViews[] = {
			// Camera
			cameraBuffer->createView<ForwardCamera>(0, cameraBuffer->getSize()),

			// Position
			staticGeometryBuffer->createView<float32>(0, sceneAsset->staticPositionSize),

			// Normal
			staticGeometryBuffer->createView<float32>(staticPositionAlignedRangeSize, sceneAsset->staticNormalSize),

			// Texture coordinate
			staticGeometryBuffer->createView<float32>(staticPositionAlignedRangeSize + staticNormalAlignedRangeSize, sceneAsset->staticTextureCoordinateSize),

			// TODO(forward)
			// Dynamic triangle tangent
			// geometryBuffer->createView<float32>(scene->getTriangleTangentComponentRange().offset, scene->getTriangleTangentComponentRange().count),
			staticGeometryBuffer->createView<float32>(0, sceneAsset->staticPositionSize),
		};

		const DescriptorImage lightmapAtlasDescriptorImage = {
			.imageView = lightmapAtlasImageView,
			.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
			.sampler = lightmapAtlasSampler,
		};

		const WriteDescriptorSet writeDescriptorSets[] = {
			// Dynamic geometry
			{
				.descriptorSet = forwardDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = descriptorBufferViews[0],
			},
			{
				.descriptorSet = forwardDescriptorSet,
				.bindingIndex = 1,
				.descriptorBufferViews = descriptorBufferViews[1],
			},
			{
				.descriptorSet = forwardDescriptorSet,
				.bindingIndex = 2,
				.descriptorBufferViews = descriptorBufferViews[2],
			},
			{
				.descriptorSet = forwardDescriptorSet,
				.bindingIndex = 3,
				.descriptorBufferViews = descriptorBufferViews[3],
			},
			{
				.descriptorSet = forwardDescriptorSet,
				.bindingIndex = 4,
				.descriptorBufferViews = descriptorBufferViews[4],
			},

			// Static geometry
			{
				.descriptorSet = staticGeometryDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = descriptorBufferViews[0],
			},
			{
				.descriptorSet = staticGeometryDescriptorSet,
				.bindingIndex = 1,
				.descriptorBufferViews = descriptorBufferViews[1],
			},
			{
				.descriptorSet = staticGeometryDescriptorSet,
				.bindingIndex = 2,
				.descriptorBufferViews = descriptorBufferViews[2],
			},
			{
				.descriptorSet = staticGeometryDescriptorSet,
				.bindingIndex = 3,
				.descriptorBufferViews = descriptorBufferViews[3],
			},
			{
				.descriptorSet = staticGeometryDescriptorSet,
				.bindingIndex = 4,
				.descriptorImages = lightmapAtlasDescriptorImage,
			},
		};

		uint32 writeDescriptorSetCount = 5;

		if (sceneAsset->lightmapAtlasImageAsset != nullptr) {
			writeDescriptorSetCount = 10;
		}

		device->updateDescriptorSets(view<WriteDescriptorSet>(writeDescriptorSets, writeDescriptorSetCount));
	}
}