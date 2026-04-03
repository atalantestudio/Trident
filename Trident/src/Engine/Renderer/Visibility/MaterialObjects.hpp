// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	static void countImageDescriptors(view<DescriptorSetLayoutBinding> descriptorSetLayoutBindings, uint32& imageDescriptorCount) {
		for (const DescriptorSetLayoutBinding& descriptorSetLayoutBinding : descriptorSetLayoutBindings) {
			ENGINE_ASSERT(descriptorSetLayoutBinding.descriptorCount == 1, "Descriptor counts larger than 1 are not supported.");
			ENGINE_ASSERT(descriptorSetLayoutBinding.shaderStages == ShaderStage::COMPUTE, "Invalid descriptor shader stage flags.");

			switch (descriptorSetLayoutBinding.descriptorType) {
				case DescriptorType::COMBINED_IMAGE_SAMPLER: {
					imageDescriptorCount += descriptorSetLayoutBinding.descriptorCount;

					break;
				}
				case DescriptorType::UNIFORM_BUFFER: {
					ENGINE_ERROR_UNIMPLEMENTED();

					break;
				}
				default: {
					ENGINE_ERROR_UNSUPPORTED();

					break;
				}
			}
		}
	}

	static uint64 copyImageAssetDataToBuffer(ref<ImageAsset> imageAsset, uint8* mappedRange) {
		const Extent3D extent = imageAsset->getExtent();

		sequence<uint8> data(extent.width * extent.height * extent.depth * imageAsset->getLayerCount() * getChannelCount(ImageFormat::R8G8B8A8_UNORM));

		switch (imageAsset->getFormat()) {
			case ImageFormat::R8G8B8_UNORM: {
				convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(imageAsset->getData(), &data[0], data.count());

				break;
			}
			case ImageFormat::R8G8B8A8_UNORM: {
				std::copy(imageAsset->getData().begin(), imageAsset->getData().end(), &data[0]);

				break;
			}
			default: {
				ENGINE_ERROR_UNSUPPORTED();

				break;
			}
		}

		std::copy(data.begin(), data.end(), mappedRange);

		return data.count();
	}

	/// 
	/// A material shader is a compute shader module written by the user. It contains 2 descriptor sets:
	/// - The first descriptor set (MaterialBase) is automatically set by the renderer. It contains everything needed to access the visibility of a pixel,
	/// as well as additional buffers to extract the triangle, cluster, geometry and instance data from this visibility.
	/// - The second ("user") descriptor is authored by the user. It contains the necessary data for the shader to produce the desired output.
	/// As of now, the bindings in this descriptor must be of type COMBINED_IMAGE_SAMPLER (TODO).
	/// For example, a PBR shader will likely require AO, roughness, metallic, diffuse and normal textures.
	/// This user descriptor set also contains the material offset buffer, which describes the range of coordinate indices referencing this particular material shader.
	/// 
	/// A material is a combination of a material shader and a list of actual images to put in the user descriptor set.
	/// This means two different PBR objects will use the same material shader (and thus the same descriptor set layout, pipeline and pipeline layout).
	/// However, the user descriptor set is tied to a single material, so it must be changed in-between dispatch calls.
	/// 
	/// The images provided to a material shader must be handled with care. The user descriptor set layout dictates where materials must provide an image.
	/// Two materials sharing the same shader will usually provide images of the same size and format. Thus, an image is created for every image binding in the user descriptor set layout.
	/// This image is layered, with each layer corresponding to the actual image provided by a material, in order. This helps reduce the total number of images created.
	/// 
	void VisibilityRenderer::createMaterialObjects() {
		const ref<Device> device = getDevice();

		AssetLibrary* assetLibrary = getLayer()->getApplication()->getAssetLibrary();

		// TODO: The user layouts will still be created if they don't have any descriptors. Is this expected?
		materialShaderUserDescriptorSetLayouts = sequence<ref<DescriptorSetLayout>>(scene->materialShaders.size());

		materialShaderPipelineLayouts = sequence<ref<PipelineLayout>>(scene->materialShaders.size());
		materialShaderPipelines = sequence<ref<ComputePipeline>>(scene->materialShaders.size());
		materialShaderUserDescriptorSets = sequence<ref<DescriptorSet>>(scene->materialShaders.size());
		materialShaderImages = sequence<sequence<ref<Image>>>(scene->materialShaders.size());
		materialShaderImageViews = sequence<sequence<ref<ImageView>>>(scene->materialShaders.size());

		// One image asset per material per descriptor set layout binding per material shader.
		sequence<sequence<sequence<ref<ImageAsset>>>> imageAssets(scene->materialShaders.size());

		const DescriptorSetLayoutBinding materialOffsetDescriptorSetLayoutBinding = {
			.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
			.descriptorCount = 1,
			.shaderStages = ShaderStage::COMPUTE,
		};

		PipelineLayoutDescriptor materialShaderPipelineLayoutDescriptor = {
			.descriptorSetLayouts = sequence<ref<DescriptorSetLayout>>(2),
		};

		ShaderModuleDescriptor materialShaderModuleDescriptor = {
			.stage = ShaderStage::COMPUTE,
			.entryPoint = "main",
		};

		ComputePipelineDescriptor materialShaderPipelineDescriptor = {};

		ImageDescriptor imageDescriptor = {
			.type = ImageType::TEXTURE_2D,
			.format = ImageFormat::R8G8B8A8_UNORM,
			.extent = {1024, 1024, 1},
			.mipLevelCount = 1,
			.sampleCount = 1,
			.usage = ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED,
		};

		ImageViewDescriptor imageViewDescriptor = {
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
		};

		uint32 materialShaderIndex = 0;
		uint32 materialShaderImageCopyRegionCount = 0;
		uint32 materialShaderDescriptorSetLayoutBindingCount = 0;
		uint64 imageStagingBufferSize = 0;

		#if defined(BUILD_MODE_DEBUG)
			sequence<char8> materialShaderName;
			sequence<char8> materialShaderModuleName;
			sequence<char8> materialShaderDescriptorSetLayoutBindingName;
		#endif

		sequence<DescriptorSetLayoutBinding> materialShaderUserDescriptorSetLayoutsBindings;

		while (materialShaderIndex < scene->materialShaders.size()) {
			const Shader& materialShader = scene->materialShaders[materialShaderIndex];
			const std::vector<Material>& materials = scene->materials[materialShaderIndex];

			#if defined(BUILD_MODE_DEBUG)
				materialShaderModuleName = Logger::format("Material shader [] compute", materialShaderIndex);
				materialShaderName = Logger::format("Material shader []", materialShaderIndex);

				materialShaderPipelineLayoutDescriptor.name = materialShaderName;
				materialShaderModuleDescriptor.name = materialShaderModuleName;
				materialShaderPipelineDescriptor.name = materialShaderName;
			#endif

			materialShaderUserDescriptorSetLayoutsBindings = sequence<DescriptorSetLayoutBinding>(materialShader.descriptorSetLayoutBindings.count() + 1);
			materialShaderUserDescriptorSetLayoutsBindings[0] = materialOffsetDescriptorSetLayoutBinding;

			if (materialShader.descriptorSetLayoutBindings.count() > 0) {
				std::copy(materialShader.descriptorSetLayoutBindings.begin(), materialShader.descriptorSetLayoutBindings.end(), &materialShaderUserDescriptorSetLayoutsBindings[1]);
			}

			materialShaderUserDescriptorSetLayouts[materialShaderIndex] = createDescriptorSetLayout(materialShaderUserDescriptorSetLayoutsBindings);

			materialShaderPipelineLayoutDescriptor.descriptorSetLayouts[0] = materialBaseDescriptorSetLayout;
			materialShaderPipelineLayoutDescriptor.descriptorSetLayouts[1] = materialShaderUserDescriptorSetLayouts[materialShaderIndex];

			materialShaderPipelineLayouts[materialShaderIndex] = createPipelineLayout(materialShaderPipelineLayoutDescriptor);

			materialShaderPipelineDescriptor.pipelineLayout = materialShaderPipelineLayouts[materialShaderIndex];
			materialShaderPipelineDescriptor.computeShaderModule = loadInternalShaderModule(materialShaderModuleDescriptor, materialShader.modulePath);

			materialShaderPipelines[materialShaderIndex] = createComputePipeline(materialShaderPipelineDescriptor);

			uint32 imageDescriptorCount = 0;

			countImageDescriptors(materialShader.descriptorSetLayoutBindings, imageDescriptorCount);

			if (imageDescriptorCount == 0) {
				materialShaderIndex += 1;

				continue;
			}

			materialShaderImages[materialShaderIndex] = sequence<ref<Image>>(imageDescriptorCount);
			materialShaderImageViews[materialShaderIndex] = sequence<ref<ImageView>>(imageDescriptorCount);
			imageAssets[materialShaderIndex] = sequence<sequence<ref<ImageAsset>>>(materialShader.descriptorSetLayoutBindings.count());

			uint32 descriptorSetLayoutBindingIndex = 0;

			while (descriptorSetLayoutBindingIndex < materialShader.descriptorSetLayoutBindings.count()) {
				const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = materialShader.descriptorSetLayoutBindings[descriptorSetLayoutBindingIndex];

				if (descriptorSetLayoutBinding.descriptorType != DescriptorType::COMBINED_IMAGE_SAMPLER) {
					descriptorSetLayoutBindingIndex += 1;

					continue;
				}

				#if defined(BUILD_MODE_DEBUG)
					materialShaderDescriptorSetLayoutBindingName = Logger::format("Material [], binding []", materialShaderIndex, descriptorSetLayoutBindingIndex);

					imageDescriptor.name = materialShaderDescriptorSetLayoutBindingName;
					imageViewDescriptor.name = materialShaderDescriptorSetLayoutBindingName;
				#endif

				imageDescriptor.layerCount = static_cast<uint32>(materials.size());

				const ref<Image> image = createImage(imageDescriptor);

				materialShaderImages[materialShaderIndex][descriptorSetLayoutBindingIndex] = image;

				imageViewDescriptor.layerCount = image->getLayerCount();

				materialShaderImageViews[materialShaderIndex][descriptorSetLayoutBindingIndex] = image->createView(imageViewDescriptor);

				imageAssets[materialShaderIndex][descriptorSetLayoutBindingIndex] = sequence<ref<ImageAsset>>(materials.size());

				uint32 materialIndex = 0;

				// Load image assets and update the size needed for the material image staging buffer.
				while (materialIndex < materials.size()) {
					const Material& material = materials[materialIndex];
					const MaterialWriteDescriptorSetBinding& materialWriteDescriptorSetBinding = material.writeDescriptorSetBindings[descriptorSetLayoutBindingIndex];

					ENGINE_ASSERT(materialWriteDescriptorSetBinding.descriptorImages.count() == descriptorSetLayoutBinding.descriptorCount, "Invalid descriptor image count.");

					// TODO(material-graph/multiple-image-descriptors): Add support for multiple image descriptors.
					const ref<ImageAsset> imageAsset = assetLibrary->getAsset<ImageAsset>(materialWriteDescriptorSetBinding.descriptorImages[0].assetPath);
					const Extent3D imageAssetExtent = imageAsset->getExtent();

					imageAssets[materialShaderIndex][descriptorSetLayoutBindingIndex][materialIndex] = imageAsset;

					// TODO: Handle other image formats.
					imageStagingBufferSize += imageAssetExtent.width * imageAssetExtent.height * imageAssetExtent.depth * imageAsset->getLayerCount() * getByteSize(ImageFormat::R8G8B8A8_UNORM);

					materialShaderImageCopyRegionCount += 1;
					materialIndex += 1;
				}

				materialShaderDescriptorSetLayoutBindingCount += 1;
				descriptorSetLayoutBindingIndex += 1;
			}

			materialShaderIndex += 1;
		}

		if (imageStagingBufferSize == 0) {
			// No images to copy.
			return;
		}

		materialShaderUserDescriptorSets = device->allocateDescriptorSets(materialShaderUserDescriptorSetLayouts);

		const ref<Buffer<uint8>> imageStagingBuffer = createStagingBuffer<uint8>(imageStagingBufferSize);

		uint8* imageMappedRange = imageStagingBuffer->map(0, imageStagingBuffer->getSize());
		uint64 imageMappedRangeOffset = 0;

		sequence<DescriptorBuffer> materialOffsetDescriptorBuffers(materialShaderUserDescriptorSets.count());
		sequence<DescriptorImage> descriptorImages(materialShaderImageCopyRegionCount);
		sequence<WriteDescriptorSet> writeDescriptorSets(materialShaderDescriptorSetLayoutBindingCount + materialShaderUserDescriptorSets.count());

		ImageViewDescriptor materialReferenceBindingImageViewDescriptor = {
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
		};

		const ref<Queue> graphicsQueue = device->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Copy material shader images",
			#endif
		});

		commandEncoder->begin();

		materialShaderIndex = 0;

		uint32 descriptorImageIndex = 0;
		uint32 writeDescriptorSetIndex = 0;

		while (materialShaderIndex < scene->materialShaders.size()) {
			const Shader& materialShader = scene->materialShaders[materialShaderIndex];
			const std::vector<Material>& materials = scene->materials[materialShaderIndex];

			materialOffsetDescriptorBuffers[materialShaderIndex] = {
				.buffer = materialOffsetBuffer,
				.offset = materialShaderIndex * sizeof(uint32),
				.size = 2 * sizeof(uint32),
			};

			writeDescriptorSets[writeDescriptorSetIndex] = {
				.descriptorSet = materialShaderUserDescriptorSets[materialShaderIndex],
				.bindingIndex = 0,
				.descriptorBuffers = view<DescriptorBuffer>(&materialOffsetDescriptorBuffers[materialShaderIndex], 1),
			};

			writeDescriptorSetIndex += 1;

			uint32 descriptorSetLayoutBindingIndex = 0;
			uint32 imageViewIndex = 0;

			while (descriptorSetLayoutBindingIndex < materialShader.descriptorSetLayoutBindings.count()) {
				const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = materialShader.descriptorSetLayoutBindings[descriptorSetLayoutBindingIndex];

				if (descriptorSetLayoutBinding.descriptorType != DescriptorType::COMBINED_IMAGE_SAMPLER) {
					descriptorSetLayoutBindingIndex += 1;

					continue;
				}

				const ref<Image> image = materialShaderImages[materialShaderIndex][descriptorSetLayoutBindingIndex];

				sequence<BufferImageCopyRegion> imageCopyRegions(materials.size());
				uint32 materialIndex = 0;

				while (materialIndex < materials.size()) {
					const ref<ImageAsset> imageAsset = imageAssets[materialShaderIndex][descriptorSetLayoutBindingIndex][materialIndex];
					const uint64 imageSize = copyImageAssetDataToBuffer(imageAsset, &imageMappedRange[imageMappedRangeOffset]);

					imageCopyRegions[materialIndex] = {
						.bufferOffset = imageMappedRangeOffset,
						.imageOffsetX = 0,
						.imageOffsetY = 0,
						.imageOffsetZ = 0,
						.imageExtent = imageAsset->getExtent(),
						.imageMipLevel = 0,
						.imageLayerOffset = materialIndex,
						.imageLayerCount = 1,
					};

					imageMappedRangeOffset += imageSize;
					materialIndex += 1;
				}

				descriptorImages[descriptorImageIndex] = {
					.imageView = materialShaderImageViews[materialShaderIndex][imageViewIndex],
					.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
					.sampler = materialShaderImageSampler,
				};

				writeDescriptorSets[writeDescriptorSetIndex] = {
					.descriptorSet = materialShaderUserDescriptorSets[materialShaderIndex],
					.bindingIndex = descriptorSetLayoutBindingIndex + 1,
					.descriptorImages = view<DescriptorImage>(&descriptorImages[descriptorImageIndex], 1),
				};

				// TODO: Set all image memory barriers at once.
				commandEncoder->setImageMemoryBarrier(image, ImageLayout::TRANSFER_DST_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

				commandEncoder->copyBufferToImage(imageStagingBuffer, image, imageCopyRegions);

				// TODO: Set all image memory barriers at once.
				commandEncoder->setImageMemoryBarrier(image, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

				imageViewIndex += 1;
				descriptorImageIndex += 1;
				writeDescriptorSetIndex += 1;

				descriptorSetLayoutBindingIndex += 1;
			}

			materialShaderIndex += 1;
		}

		imageStagingBuffer->unmap();

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		graphicsQueue->submit({commandBuffer});
		graphicsQueue->waitIdle();

		device->updateDescriptorSets(writeDescriptorSets);
	}
}