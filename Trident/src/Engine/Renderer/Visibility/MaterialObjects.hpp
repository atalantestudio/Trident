// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	void VisibilityRenderer::createMaterialObjects() {
		// TODO: Use a larger extent, e.g. 4096 * 4096 * 1.
		static constexpr Extent3D IMAGE_EXTENT = {1024, 1024, 1};

		const ref<Device> device = getDevice();
		const view<Material> materials = scene->materials;

		materialDescriptorSetLayouts = sequence<ref<DescriptorSetLayout>>(materials.count());
		materialDescriptorSets = sequence<ref<DescriptorSet>>(materials.count());
		materialPipelineLayouts = sequence<ref<PipelineLayout>>(materials.count());
		materialPipelines = sequence<ref<ComputePipeline>>(materials.count());
		materialImages = sequence<sequence<ref<Image>>>(materials.count());

		// Each material has N bindings. 1 image asset is specified per binding for each material reference.
		sequence<sequence<sequence<ref<ImageAsset>>>> materialImageAssets(materials.count());

		PipelineLayoutDescriptor materialPipelineLayoutDescriptor{};

		ShaderModuleDescriptor materialComputeShaderModuleDescriptor{};
		materialComputeShaderModuleDescriptor.stage = ShaderStage::COMPUTE;
		materialComputeShaderModuleDescriptor.entryPoint = "main";

		ComputePipelineDescriptor materialPipelineDescriptor{};

		ImageDescriptor materialDescriptorSetLayoutBindingImageDescriptor{};
		materialDescriptorSetLayoutBindingImageDescriptor.type = ImageType::TEXTURE_2D;
		materialDescriptorSetLayoutBindingImageDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
		materialDescriptorSetLayoutBindingImageDescriptor.extent = IMAGE_EXTENT;
		materialDescriptorSetLayoutBindingImageDescriptor.mipLevelCount = 1;
		materialDescriptorSetLayoutBindingImageDescriptor.sampleCount = 1;
		materialDescriptorSetLayoutBindingImageDescriptor.usage = ImageUsage::SAMPLED | ImageUsage::TRANSFER_DST;
		materialDescriptorSetLayoutBindingImageDescriptor.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED;

		#if defined(BUILD_MODE_DEBUG)
			sequence<char8> name;
		#endif

		uint32 materialIndex = 0;
		uint32 bindingIndex = 0;
		uint32 materialBindingCount = 0;
		uint32 materialReferenceIndex = 0;
		uint32 imageCopyRegionCount = 0;
		uint64 imageStagingBufferSize = 0;

		while (materialIndex < materials.count()) {
			const Material& material = materials[materialIndex];
			const view<MaterialReference> materialReferences = scene->materialReferences[materialIndex];

			#if defined(BUILD_MODE_DEBUG)
				name = Logger::format("Material [] compute", materialIndex);

				SET_DEBUG_NAME(materialComputeShaderModuleDescriptor, name);

				name = Logger::format("Material []", materialIndex);

				SET_DEBUG_NAME(materialPipelineLayoutDescriptor, name);
				SET_DEBUG_NAME(materialPipelineDescriptor, name);
			#endif

			if (material.descriptorSetLayoutBindings.count() > 0) {
				materialDescriptorSetLayouts[materialIndex] = createDescriptorSetLayout(material.descriptorSetLayoutBindings);

				materialPipelineLayoutDescriptor.descriptorSetLayouts = {materialBaseDescriptorSetLayout, materialDescriptorSetLayouts[materialIndex]};
			}
			else {
				materialPipelineLayoutDescriptor.descriptorSetLayouts = {materialBaseDescriptorSetLayout};
			}

			materialPipelineLayouts[materialIndex] = createPipelineLayout(materialPipelineLayoutDescriptor);

			materialPipelineDescriptor.pipelineLayout = materialPipelineLayouts[materialIndex];
			materialPipelineDescriptor.computeShaderModule = loadInternalShaderModule(materialComputeShaderModuleDescriptor, material.shaderModulePath);

			materialPipelines[materialIndex] = createComputePipeline(materialPipelineDescriptor);

			materialImageAssets[materialIndex] = sequence<sequence<ref<ImageAsset>>>(material.descriptorSetLayoutBindings.count());
			materialImages[materialIndex] = sequence<ref<Image>>(material.descriptorSetLayoutBindings.count());

			bindingIndex = 0;

			while (bindingIndex < material.descriptorSetLayoutBindings.count()) {
				const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = material.descriptorSetLayoutBindings[bindingIndex];

				DEBUG_ASSERT(descriptorSetLayoutBinding.descriptorType == DescriptorType::COMBINED_IMAGE_SAMPLER);
				DEBUG_ASSERT(descriptorSetLayoutBinding.shaderStages == ShaderStage::COMPUTE);

				materialDescriptorSetLayoutBindingImageDescriptor.layerCount = descriptorSetLayoutBinding.descriptorCount;

				#if defined(BUILD_MODE_DEBUG)
					name = Logger::format("Material [], binding []", materialIndex, bindingIndex);

					SET_DEBUG_NAME(materialDescriptorSetLayoutBindingImageDescriptor, name);
				#endif

				materialImages[materialIndex][bindingIndex] = createImage(materialDescriptorSetLayoutBindingImageDescriptor);
				materialImageAssets[materialIndex][bindingIndex] = sequence<ref<ImageAsset>>(materialReferences.count());

				materialReferenceIndex = 0;

				while (materialReferenceIndex < materialReferences.count()) {
					const MaterialReference& materialReference = materialReferences[materialReferenceIndex];
					const ref<ImageAsset> imageAsset = getLayer()->getApplication()->getAssetLibrary()->getAsset<ImageAsset>(materialReference.descriptorImageAssetPaths[bindingIndex]);

					materialImageAssets[materialIndex][bindingIndex][materialReferenceIndex] = imageAsset;

					imageStagingBufferSize += imageAsset->getExtent().width * imageAsset->getExtent().height * imageAsset->getExtent().depth * imageAsset->getLayerCount() * 4;

					imageCopyRegionCount += 1;
					materialReferenceIndex += 1;
				}

				bindingIndex += 1;
				materialBindingCount += 1;
			}

			materialIndex += 1;
		}

		if (imageStagingBufferSize == 0) {
			return;
		}

		materialDescriptorSets = device->allocateDescriptorSets(materialDescriptorSetLayouts);

		const ref<Buffer> imageStagingBuffer = createStagingBuffer(imageStagingBufferSize);

		uint8* imageMappedRange = imageStagingBuffer->map<uint8>(0, imageStagingBuffer->getSize());
		uint64 imageMappedRangeOffset = 0;
		uint32 materialReferenceBindingImageIndex = 0;

		materialReferenceBindingImageViews = sequence<ref<ImageView>>(imageCopyRegionCount);

		sequence<BufferImageCopyRegion> imageCopyRegions(imageCopyRegionCount);
		sequence<DescriptorImage> descriptorImages(imageCopyRegionCount);
		sequence<WriteDescriptorSet> writeDescriptorSets(materialBindingCount);

		materialIndex = 0;
		materialBindingCount = 0;

		ImageViewDescriptor materialReferenceBindingImageViewDescriptor{};
		materialReferenceBindingImageViewDescriptor.mipLevelOffset = 0;
		materialReferenceBindingImageViewDescriptor.mipLevelCount = 1;
		materialReferenceBindingImageViewDescriptor.layerCount = 1;

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		SET_DEBUG_NAME(commandBufferDescriptor, "Write material images");

		const ref<Queue> queue = device->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();

		while (materialIndex < materials.count()) {
			const Material& material = materials[materialIndex];
			const view<MaterialReference> materialReferences = scene->materialReferences[materialIndex];

			bindingIndex = 0;

			while (bindingIndex < material.descriptorSetLayoutBindings.count()) {
				const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = material.descriptorSetLayoutBindings[bindingIndex];
				const ref<Image> image = materialImages[materialIndex][bindingIndex];
				const view<DescriptorImage> bindingDescriptorImages(&descriptorImages[materialReferenceBindingImageIndex], materialReferences.count());

				sequence<BufferImageCopyRegion> imageCopyRegions(materialReferences.count());

				materialReferenceIndex = 0;

				while (materialReferenceIndex < materialReferences.count()) {
					const ref<ImageAsset> imageAsset = materialImageAssets[materialIndex][bindingIndex][materialReferenceIndex];

					DEBUG_ASSERT(imageAsset->getFormat() == ImageFormat::R8G8B8_UNORM);

					sequence<uint8> data(imageAsset->getExtent().width * imageAsset->getExtent().height * imageAsset->getExtent().depth * imageAsset->getLayerCount() * 4);

					convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(imageAsset->getData(), &data[0], data.count());

					std::copy(data.begin(), data.end(), &imageMappedRange[imageMappedRangeOffset]);

					BufferImageCopyRegion& imageCopyRegion = imageCopyRegions[materialReferenceIndex];
					imageCopyRegion.bufferOffset = imageMappedRangeOffset;
					imageCopyRegion.imageOffsetX = 0;
					imageCopyRegion.imageOffsetY = 0;
					imageCopyRegion.imageOffsetZ = 0;
					imageCopyRegion.imageExtent = imageAsset->getExtent();
					imageCopyRegion.imageMipLevel = 0;
					imageCopyRegion.imageLayerOffset = materialReferenceIndex;
					imageCopyRegion.imageLayerCount = 1;

					materialReferenceBindingImageViewDescriptor.layerOffset = materialReferenceIndex;

					#if defined(BUILD_MODE_DEBUG)
						name = Logger::format("Material [], binding [], reference []", materialIndex, bindingIndex, materialReferenceIndex);

						SET_DEBUG_NAME(materialReferenceBindingImageViewDescriptor, name);
					#endif

					materialReferenceBindingImageViews[materialReferenceBindingImageIndex] = image->createView(materialReferenceBindingImageViewDescriptor);

					DescriptorImage& descriptorImage = descriptorImages[materialReferenceBindingImageIndex];
					descriptorImage.imageView = materialReferenceBindingImageViews[materialReferenceBindingImageIndex];
					descriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
					descriptorImage.sampler = materialImageSampler;

					imageMappedRangeOffset += data.count();
					materialReferenceIndex += 1;
					materialReferenceBindingImageIndex += 1;
				}

				WriteDescriptorSet& writeDescriptorSet = writeDescriptorSets[bindingIndex];
				writeDescriptorSet.descriptorSet = materialDescriptorSets[materialIndex];
				writeDescriptorSet.bindingIndex = bindingIndex;
				writeDescriptorSet.descriptorImages = bindingDescriptorImages;

				commandEncoder->setImageMemoryBarrier(image, ImageLayout::TRANSFER_DST_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
				commandEncoder->copyBufferToImage(imageStagingBuffer, image, imageCopyRegions);
				commandEncoder->setImageMemoryBarrier(image, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

				bindingIndex += 1;
				materialBindingCount += 1;
			}

			materialIndex += 1;
		}

		imageStagingBuffer->unmap();

		queue->submit({commandEncoder->end()});
		queue->waitIdle();

		device->updateDescriptorSets(writeDescriptorSets);
	}
}