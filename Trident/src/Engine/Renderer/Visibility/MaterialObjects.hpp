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

	/// 
	/// A material shader is a compute shader module written by the user. It contains 2 descriptor sets.
	/// 
	/// The first descriptor set (MaterialBase) is automatically set by the renderer. It contains everything needed to access the visibility of a pixel,
	/// as well as additional buffers to extract the triangle, cluster, geometry and instance data from this visibility.
	/// 
	/// The second ("user") descriptor is authored by the user. It contains the necessary data for the shader to produce the desired output.
	/// As of now, the bindings in this descriptor must be of type COMBINED_IMAGE_SAMPLER (TODO).
	/// For example, a PBR shader will likely require AO, roughness, metallic, diffuse and normal textures.
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
		// TODO: Add support for larger extents.
		constexpr Extent3D MAX_IMAGE_EXTENT = {1024, 1024, 1};

		const ref<Device> device = getDevice();

		AssetLibrary* assetLibrary = getLayer()->getApplication()->getAssetLibrary();

		// TODO: Layouts will still be created if the shader doesn't have client descriptor sets. Is this wanted?
		materialShaderUserDescriptorSetLayouts = sequence<ref<DescriptorSetLayout>>(scene->materialShaders.size());

		materialShaderPipelineLayouts = sequence<ref<PipelineLayout>>(scene->materialShaders.size());
		materialShaderPipelines = sequence<ref<ComputePipeline>>(scene->materialShaders.size());
		materialShaderUserDescriptorSets = sequence<ref<DescriptorSet>>(scene->materialShaders.size());
		materialShaderImages = sequence<sequence<ref<Image>>>(scene->materialShaders.size());

		// One image asset per material per descriptor set layout binding per material shader.
		sequence<sequence<sequence<ref<ImageAsset>>>> materialImageAssets(scene->materialShaders.size());

		PipelineLayoutDescriptor materialShaderPipelineLayoutDescriptor = {
			.descriptorSetLayouts = sequence<ref<DescriptorSetLayout>>(2),
		};

		ShaderModuleDescriptor materialShaderModuleDescriptor = {
			.stage = ShaderStage::COMPUTE,
			.entryPoint = "main",
		};

		ComputePipelineDescriptor materialShaderPipelineDescriptor = {};

		ImageDescriptor materialShaderImageDescriptor = {
			.type = ImageType::TEXTURE_2D,
			.format = ImageFormat::R8G8B8A8_UNORM,
			.extent = MAX_IMAGE_EXTENT,
			.mipLevelCount = 1,
			.sampleCount = 1,
			.usage = ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED,
		};

		#if defined(BUILD_MODE_DEBUG)
			sequence<char8> materialShaderName;
			sequence<char8> materialShaderModuleName;
			sequence<char8> materialShaderDescriptorSetLayoutBindingName;
		#endif

		uint32 materialShaderIndex = 0;
		uint32 materialShaderImageCopyRegionCount = 0;
		uint32 materialShaderDescriptorSetLayoutBindingCount = 0;
		uint64 materialShaderImageStagingBufferSize = 0;

		while (materialShaderIndex < scene->materialShaders.size()) {
			const Shader& materialShader = scene->materialShaders[materialShaderIndex];
			const std::vector<Material>& materials = scene->materials[materialShaderIndex];

			ENGINE_TRACE("In material shader [].", materialShaderIndex);

			#if defined(BUILD_MODE_DEBUG)
				materialShaderModuleName = Logger::format("Material shader [] compute", materialShaderIndex);
				materialShaderName = Logger::format("Material shader []", materialShaderIndex);

				materialShaderPipelineLayoutDescriptor.name = materialShaderName;
				materialShaderModuleDescriptor.name = materialShaderModuleName;
				materialShaderPipelineDescriptor.name = materialShaderName;
			#endif

			materialShaderUserDescriptorSetLayouts[materialShaderIndex] = createDescriptorSetLayout(materialShader.descriptorSetLayoutBindings);

			ENGINE_TRACE("\tCreated descriptor set layout for material shader [].", materialShaderIndex);

			materialShaderPipelineLayoutDescriptor.descriptorSetLayouts[0] = materialBaseDescriptorSetLayout;
			materialShaderPipelineLayoutDescriptor.descriptorSetLayouts[1] = materialShaderUserDescriptorSetLayouts[materialShaderIndex];

			materialShaderPipelineLayouts[materialShaderIndex] = createPipelineLayout(materialShaderPipelineLayoutDescriptor);

			ENGINE_TRACE("\tCreated pipeline layout for material shader [].", materialShaderIndex);

			materialShaderPipelineDescriptor.pipelineLayout = materialShaderPipelineLayouts[materialShaderIndex];
			materialShaderPipelineDescriptor.computeShaderModule = loadInternalShaderModule(materialShaderModuleDescriptor, materialShader.modulePath);

			materialShaderPipelines[materialShaderIndex] = createComputePipeline(materialShaderPipelineDescriptor);

			ENGINE_TRACE("\tCreated pipeline for material shader [].", materialShaderIndex);

			uint32 imageDescriptorCount = 0;

			countImageDescriptors(materialShader.descriptorSetLayoutBindings, imageDescriptorCount);

			ENGINE_TRACE("\tMaterial shader [] has [] image descriptors.", materialShaderIndex, imageDescriptorCount);

			if (imageDescriptorCount == 0) {
				materialShaderIndex += 1;

				continue;
			}

			materialShaderImages[materialShaderIndex] = sequence<ref<Image>>(imageDescriptorCount);
			materialImageAssets[materialShaderIndex] = sequence<sequence<ref<ImageAsset>>>(materialShader.descriptorSetLayoutBindings.count());

			ENGINE_TRACE("\tPreinitialized [] images for material shader [].", imageDescriptorCount, materialShaderIndex);
			ENGINE_TRACE("\tPrepared [] sequences of image assets for material shader [].", materialShader.descriptorSetLayoutBindings.count(), materialShaderIndex);

			uint32 descriptorSetLayoutBindingIndex = 0;

			while (descriptorSetLayoutBindingIndex < materialShader.descriptorSetLayoutBindings.count()) {
				const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = materialShader.descriptorSetLayoutBindings[descriptorSetLayoutBindingIndex];

				ENGINE_TRACE("\tIn descriptor set layout binding [] for material shader [].", descriptorSetLayoutBindingIndex, materialShaderIndex);

				if (descriptorSetLayoutBinding.descriptorType != DescriptorType::COMBINED_IMAGE_SAMPLER) {
					descriptorSetLayoutBindingIndex += 1;

					continue;
				}

				#if defined(BUILD_MODE_DEBUG)
					materialShaderDescriptorSetLayoutBindingName = Logger::format("Material [], binding []", materialShaderIndex, descriptorSetLayoutBindingIndex);

					materialShaderImageDescriptor.name = materialShaderDescriptorSetLayoutBindingName;
				#endif

				materialShaderImageDescriptor.layerCount = static_cast<uint32>(materials.size());

				materialShaderImages[materialShaderIndex][descriptorSetLayoutBindingIndex] = createImage(materialShaderImageDescriptor);

				materialImageAssets[materialShaderIndex][descriptorSetLayoutBindingIndex] = sequence<ref<ImageAsset>>(materials.size());

				ENGINE_TRACE("\t\tPrepared [] image assets for material shader [].", materials.size(), materialShaderIndex);

				uint32 materialIndex = 0;

				// Load image assets and update the size needed for the material image staging buffer.
				while (materialIndex < materials.size()) {
					const Material& material = materials[materialIndex];
					const MaterialWriteDescriptorSetBinding& materialWriteDescriptorSetBinding = material.writeDescriptorSetBindings[descriptorSetLayoutBindingIndex];

					ENGINE_TRACE("\t\tIn material [] of material shader [].", materialIndex, materialShaderIndex);

					ENGINE_ASSERT(materialWriteDescriptorSetBinding.descriptorImages.count() == descriptorSetLayoutBinding.descriptorCount, "Invalid descriptor image count.");

					// TODO(material-graph/multiple-image-descriptors): Add support for multiple image descriptors.
					const ref<ImageAsset> imageAsset = assetLibrary->getAsset<ImageAsset>(materialWriteDescriptorSetBinding.descriptorImages[0].assetPath);
					const Extent3D imageAssetExtent = imageAsset->getExtent();

					materialImageAssets[materialShaderIndex][descriptorSetLayoutBindingIndex][materialIndex] = imageAsset;

					// materialShaderImageStagingBufferSize += imageAssetExtent.width * imageAssetExtent.height * imageAssetExtent.depth * imageAsset->getLayerCount() * getByteSize(imageAsset->getFormat());
					materialShaderImageStagingBufferSize += imageAssetExtent.width * imageAssetExtent.height * imageAssetExtent.depth * imageAsset->getLayerCount() * getByteSize(ImageFormat::R8G8B8A8_UNORM);

					ENGINE_TRACE("\t\t\tMaterial shader image staging buffer size is now [].", materialShaderImageStagingBufferSize);

					materialShaderImageCopyRegionCount += 1;
					materialIndex += 1;
				}

				materialShaderDescriptorSetLayoutBindingCount += 1;
				descriptorSetLayoutBindingIndex += 1;
			}

			materialShaderIndex += 1;
		}

		if (materialShaderImageStagingBufferSize == 0) {
			// No image to copy.
			return;
		}

		materialShaderUserDescriptorSets = device->allocateDescriptorSets(materialShaderUserDescriptorSetLayouts);

		/* while (materialIndex < materials.count()) {
			const Material& material = materials[materialIndex];
			const view<MaterialReference> materialReferences = scene->materialReferences[materialIndex];

			#if defined(BUILD_MODE_DEBUG)
				name = Logger::format("Material [] compute", materialIndex);

				SET_DEBUG_NAME(materialComputeShaderModuleDescriptor, name);

				name1 = Logger::format("Material shader []", materialIndex);

				SET_DEBUG_NAME(materialShaderPipelineLayoutDescriptor, name1);
				SET_DEBUG_NAME(materialShaderPipelineDescriptor, name1);
			#endif

			if (material.descriptorSetLayoutBindings.count() > 0) {
				materialDescriptorSetLayouts[materialIndex] = createDescriptorSetLayout(material.descriptorSetLayoutBindings);

				materialShaderPipelineLayoutDescriptor.descriptorSetLayouts = {materialBaseDescriptorSetLayout, materialDescriptorSetLayouts[materialIndex]};
			}
			else {
				materialShaderPipelineLayoutDescriptor.descriptorSetLayouts = {materialBaseDescriptorSetLayout};
			}

			materialShaderPipelineLayouts[materialIndex] = createPipelineLayout(materialShaderPipelineLayoutDescriptor);

			materialShaderPipelineDescriptor.pipelineLayout = materialShaderPipelineLayouts[materialIndex];
			materialShaderPipelineDescriptor.computeShaderModule = loadInternalShaderModule(materialComputeShaderModuleDescriptor, material.shaderModulePath);

			materialShaderPipelines[materialIndex] = createComputePipeline(materialShaderPipelineDescriptor);

			materialImageAssets[materialIndex] = sequence<sequence<ref<ImageAsset>>>(materialShader.descriptorSetLayoutBindings.count());
			materialShaderImages[materialIndex] = sequence<ref<Image>>(materialShader.descriptorSetLayoutBindings.count());

			bindingIndex = 0;

			while (bindingIndex < material.descriptorSetLayoutBindings.count()) {
				const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = material.descriptorSetLayoutBindings[bindingIndex];

				DEBUG_ASSERT(descriptorSetLayoutBinding.descriptorType == DescriptorType::COMBINED_IMAGE_SAMPLER);
				DEBUG_ASSERT(descriptorSetLayoutBinding.shaderStages == ShaderStage::COMPUTE);

				materialShaderImageDescriptor.layerCount = descriptorSetLayoutBinding.descriptorCount;

				#if defined(BUILD_MODE_DEBUG)
					name = Logger::format("Material [], binding []", materialIndex, bindingIndex);

					SET_DEBUG_NAME(materialShaderImageDescriptor, name);
				#endif

				materialShaderImages[materialIndex][bindingIndex] = createImage(materialShaderImageDescriptor);
				materialImageAssets[materialIndex][bindingIndex] = sequence<ref<ImageAsset>>(materialReferences.count());

				materialReferenceIndex = 0;

				while (materialReferenceIndex < materialReferences.count()) {
					const MaterialReference& materialReference = materialReferences[materialReferenceIndex];
					const ref<ImageAsset> imageAsset = assetLibrary->getAsset<ImageAsset>(materialReference.descriptorImageAssetPaths[bindingIndex]);

					materialImageAssets[materialIndex][bindingIndex][materialReferenceIndex] = imageAsset;

					materialImageStagingBufferSize += imageAsset->getExtent().width * imageAsset->getExtent().height * imageAsset->getExtent().depth * imageAsset->getLayerCount() * 4;

					imageCopyRegionCount += 1;
					materialReferenceIndex += 1;
				}

				bindingIndex += 1;
				materialBindingCount += 1;
			}

			materialIndex += 1;
		}

		if (materialImageStagingBufferSize == 0) {
			return;
		} */

		materialShaderUserDescriptorSets = device->allocateDescriptorSets(materialShaderUserDescriptorSetLayouts);

		const ref<Buffer<uint8>> imageStagingBuffer = createStagingBuffer<uint8>(materialShaderImageStagingBufferSize);

		uint8* imageMappedRange = imageStagingBuffer->map(0, imageStagingBuffer->getSize());
		uint64 imageMappedRangeOffset = 0;
		uint32 materialReferenceBindingImageIndex = 0;

		materialReferenceBindingImageViews = sequence<ref<ImageView>>(materialShaderImageCopyRegionCount);

		sequence<BufferImageCopyRegion> imageCopyRegions(materialShaderImageCopyRegionCount);
		sequence<DescriptorImage> descriptorImages(materialShaderImageCopyRegionCount);
		sequence<WriteDescriptorSet> writeDescriptorSets(materialShaderDescriptorSetLayoutBindingCount);

		materialShaderIndex = 0;

		ImageViewDescriptor materialReferenceBindingImageViewDescriptor = {
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerCount = 1,
		};

		const ref<Queue> graphicsQueue = device->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(BUILD_MODE_DEBUG)
				.name = "Copy material shader images",
			#endif
		});

		commandEncoder->begin();

		while (materialShaderIndex < scene->materialShaders.size()) {
			const Shader& materialShader = scene->materialShaders[materialShaderIndex];
			const std::vector<Material>& materials = scene->materials[materialShaderIndex];

			uint32 bindingIndex = 0;

			while (bindingIndex < materialShader.descriptorSetLayoutBindings.count()) {
				const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = materialShader.descriptorSetLayoutBindings[bindingIndex];
				const ref<Image> image = materialShaderImages[materialShaderIndex][bindingIndex];
				const view<DescriptorImage> bindingDescriptorImages(&descriptorImages[materialReferenceBindingImageIndex], materials.size());

				sequence<BufferImageCopyRegion> imageCopyRegions(materials.size());

				uint32 materialReferenceIndex = 0;

				while (materialReferenceIndex < materials.size()) {
					const ref<ImageAsset> imageAsset = materialImageAssets[materialShaderIndex][bindingIndex][materialReferenceIndex];

					DEBUG_ASSERT(imageAsset->getFormat() == ImageFormat::R8G8B8_UNORM);

					sequence<uint8> data(imageAsset->getExtent().width * imageAsset->getExtent().height * imageAsset->getExtent().depth * imageAsset->getLayerCount() * 4);

					convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(imageAsset->getData(), &data[0], data.count());

					std::copy(data.begin(), data.end(), &imageMappedRange[imageMappedRangeOffset]);

					imageCopyRegions[materialReferenceIndex] = {
						.bufferOffset = imageMappedRangeOffset,
						.imageOffsetX = 0,
						.imageOffsetY = 0,
						.imageOffsetZ = 0,
						.imageExtent = imageAsset->getExtent(),
						.imageMipLevel = 0,
						.imageLayerOffset = materialReferenceIndex,
						.imageLayerCount = 1,
					};

					materialReferenceBindingImageViewDescriptor.layerOffset = materialReferenceIndex;

					#if defined(BUILD_MODE_DEBUG)
						// name = Logger::format("Material [], binding [], reference []", materialShaderIndex, bindingIndex, materialReferenceIndex);

						// SET_DEBUG_NAME(materialReferenceBindingImageViewDescriptor, name);
					#endif

					materialReferenceBindingImageViews[materialReferenceBindingImageIndex] = image->createView(materialReferenceBindingImageViewDescriptor);

					descriptorImages[materialReferenceBindingImageIndex] = {
						.imageView = materialReferenceBindingImageViews[materialReferenceBindingImageIndex],
						.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
						.sampler = materialShaderImageSampler,
					};

					imageMappedRangeOffset += data.count();
					materialReferenceIndex += 1;
					materialReferenceBindingImageIndex += 1;
				}

				WriteDescriptorSet& writeDescriptorSet = writeDescriptorSets[bindingIndex];
				writeDescriptorSet.descriptorSet = materialShaderUserDescriptorSets[materialShaderIndex];
				writeDescriptorSet.bindingIndex = bindingIndex;
				writeDescriptorSet.descriptorImages = bindingDescriptorImages;

				commandEncoder->setImageMemoryBarrier(image, ImageLayout::TRANSFER_DST_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);
				commandEncoder->copyBufferToImage(imageStagingBuffer, image, imageCopyRegions);
				commandEncoder->setImageMemoryBarrier(image, ImageLayout::SHADER_READ_ONLY_OPTIMAL, PipelineStage::ALL_COMMANDS, PipelineStage::ALL_COMMANDS);

				bindingIndex += 1;
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