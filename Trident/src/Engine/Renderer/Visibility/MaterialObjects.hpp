// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Renderer/Visibility/Renderer.hpp"

namespace Trident {
	// - Creates a descriptor set layout for each material shader
	// - Creates a pipeline layout for each material shader
	// - Creates a pipeline for each material shader
	// - Calculates the total image count and image asset count
	// - Calculates the total byte size needed for the image staging buffer
	void VisibilityRenderer::initializeMaterialShaders(uint32& imageCount, uint32& imageAssetCount, uint64& imageStagingBufferSize) {
		const DescriptorSetLayoutBinding materialOffsetBinding = {
			.descriptorType = DescriptorType::READ_ONLY_STORAGE_BUFFER,
			.descriptorCount = 1,
			.stageFlags = ShaderStage::COMPUTE,
		};

		AssetLibrary* assetLibrary = getLayer()->getApplication()->getAssetLibrary();
		PipelineLayoutDescriptor materialShaderPipelineLayoutDescriptor = {
			.descriptorSetLayouts = sequence<ref<DescriptorSetLayout>>(2),
		};
		ShaderModuleDescriptor materialShaderModuleDescriptor = {
			.stage = ShaderStage::COMPUTE,
			.entryPoint = "main",
		};
		ComputePipelineDescriptor materialShaderPipelineDescriptor = {};

		#if defined(DEBUG_OBJECT_NAME)
			sequence<char8> materialShaderName;
			sequence<char8> materialShaderModuleName;
		#endif

		uint32 materialShaderIndex = 0;

		while (materialShaderIndex < scene->materialShaders.count()) {
			const MaterialShader& materialShader = scene->materialShaders[materialShaderIndex];
			const ref<ShaderModuleAsset> materialShaderModuleAsset = materialShader.shaderModuleAsset;
			const std::vector<ref<MaterialAsset>>& materialAssets = scene->materialAssets[materialShaderIndex];

			#if defined(DEBUG_OBJECT_NAME)
				materialShaderModuleName = Logger::format("Material shader [] compute", materialShaderIndex);
				materialShaderName = Logger::format("Material shader []", materialShaderIndex);

				materialShaderPipelineLayoutDescriptor.name = materialShaderName;
				materialShaderModuleDescriptor.name = materialShaderModuleName;
				materialShaderPipelineDescriptor.name = materialShaderName;
			#endif

			/* sequence<DescriptorSetLayoutBinding> descriptorSetLayoutBindings(materialShaderModuleAsset->descriptorSetLayoutBindings.count());
			uint32 descriptorSetLayoutBindingIndex = 0;

			while (descriptorSetLayoutBindingIndex < descriptorSetLayoutBindings.count()) {
				DescriptorSetLayoutBinding& descriptorSetLayoutBinding = descriptorSetLayoutBindings[descriptorSetLayoutBindingIndex];
				descriptorSetLayoutBinding.descriptorType = materialShaderModuleAsset->descriptorSetLayoutBindings[descriptorSetLayoutBindingIndex];
				descriptorSetLayoutBinding.descriptorCount = 1;
				descriptorSetLayoutBinding.stageFlags = materialShaderModuleAsset->stage;

				descriptorSetLayoutBindingIndex += 1;
			} */

			sequence<DescriptorSetLayoutBinding> userDescriptorSetLayoutBindings(materialShaderModuleAsset->bindings.count() + 1);
			userDescriptorSetLayoutBindings[0] = materialOffsetBinding;

			if (materialShaderModuleAsset->bindings.count() > 0) {
				std::copy(materialShaderModuleAsset->bindings.begin(), materialShaderModuleAsset->bindings.end(), &userDescriptorSetLayoutBindings[1]);
			}

			materialShaderUserDescriptorSetLayouts[materialShaderIndex] = createDescriptorSetLayout({
				.bindings = userDescriptorSetLayoutBindings,
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Material shader user",
				#endif
			});

			materialShaderPipelineLayoutDescriptor.descriptorSetLayouts[0] = materialBaseDescriptorSetLayout;
			materialShaderPipelineLayoutDescriptor.descriptorSetLayouts[1] = materialShaderUserDescriptorSetLayouts[materialShaderIndex];

			materialShaderPipelineLayouts[materialShaderIndex] = createPipelineLayout(materialShaderPipelineLayoutDescriptor);

			materialShaderPipelineDescriptor.pipelineLayout = materialShaderPipelineLayouts[materialShaderIndex];

			sequence<char8> path(9 + materialShaderModuleAsset->name.count());

			std::copy_n("Material/", 9, &path[0]);
			std::copy_n(&materialShaderModuleAsset->name[0], materialShaderModuleAsset->name.count(), &path[9]);

			materialShaderPipelineDescriptor.computeShaderModule = loadInternalShaderModule(materialShaderModuleDescriptor, path);

			materialShaderPipelines[materialShaderIndex] = createComputePipeline(materialShaderPipelineDescriptor);

			uint32 descriptorSetLayoutBindingIndex = 0;

			while (descriptorSetLayoutBindingIndex < materialShaderModuleAsset->bindings.count()) {
				const DescriptorType descriptorType = materialShaderModuleAsset->bindings[descriptorSetLayoutBindingIndex].descriptorType;

				switch (descriptorType) {
					case DescriptorType::COMBINED_IMAGE_SAMPLER: {
						uint32 materialIndex = 0;

						while (materialIndex < materialAssets.size()) {
							const ref<MaterialAsset> materialAsset = materialAssets[materialIndex];
							const ref<ImageAsset> imageAsset = assetLibrary->getAsset<ImageAsset>(materialAsset->descriptorImageAssetIdentifiers[descriptorSetLayoutBindingIndex]);
							const Extent3D extent = imageAsset->getExtent();

							// TODO(material-graph/formats): Add support for other image formats (mainly R8_UNORM and R8G8B8_UNORM).
							imageStagingBufferSize += extent.x * extent.y * extent.z * imageAsset->getLayerCount() * getByteSize(ImageFormat::R8G8B8A8_UNORM);

							materialIndex += 1;
						}

						imageAssetCount += static_cast<uint32>(materialAssets.size());

						break;
					}
					default: {
						ENGINE_ERROR_UNSUPPORTED();

						break;
					}
				}

				descriptorSetLayoutBindingIndex += 1;
			}

			imageCount += static_cast<uint32>(materialShaderModuleAsset->bindings.count());

			materialShaderIndex += 1;
		}
	}

	// - Creates images
	// - Creates image views
	// - Initializes image memory barriers
	// - Initializes descriptor buffers (MaterialOffsetBuffer)
	// - Initializes descriptor images
	// - Initializes write descriptor sets for both descriptor buffers and descriptor images
	void VisibilityRenderer::initializeMaterialImages(sequence<ImageMemoryBarrier>& imageMemoryBarriers, sequence<BaseBufferView>& materialOffsetViews, sequence<DescriptorImage>& descriptorImages, sequence<WriteDescriptorSet>& writeDescriptorSets) {
		ImageDescriptor imageDescriptor = {
			.type = ImageType::TYPE_2D,
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

		#if defined(DEBUG_OBJECT_NAME)
			sequence<char8> materialShaderDescriptorSetLayoutBindingName;
		#endif

		uint32 materialShaderIndex = 0;
		uint32 imageIndex = 0;
		uint32 writeDescriptorSetIndex = 0;

		while (materialShaderIndex < scene->materialShaders.count()) {
			const MaterialShader& materialShader = scene->materialShaders[materialShaderIndex];
			const ref<ShaderModuleAsset> materialShaderModuleAsset = materialShader.shaderModuleAsset;
			const std::vector<ref<MaterialAsset>>& materialAssets = scene->materialAssets[materialShaderIndex];
			const ref<DescriptorSet> materialShaderUserDescriptorSet = materialShaderUserDescriptorSets[materialShaderIndex];

			imageDescriptor.layerCount = static_cast<uint32>(materialAssets.size());
			imageViewDescriptor.layerCount = static_cast<uint32>(materialAssets.size());

			materialOffsetViews[materialShaderIndex] = materialOffsetBuffer->createView<uint32>(materialShaderIndex * sizeof(uint32), 2 * sizeof(uint32));

			writeDescriptorSets[writeDescriptorSetIndex] = {
				.descriptorSet = materialShaderUserDescriptorSet,
				.bindingIndex = 0,
				.descriptorBufferViews = materialOffsetViews[materialShaderIndex],
			};

			writeDescriptorSetIndex += 1;

			uint32 descriptorSetLayoutBindingIndex = 0;

			while (descriptorSetLayoutBindingIndex < materialShaderModuleAsset->bindings.count()) {
				const DescriptorType descriptorType = materialShaderModuleAsset->bindings[descriptorSetLayoutBindingIndex].descriptorType;

				switch (descriptorType) {
					case DescriptorType::COMBINED_IMAGE_SAMPLER: {
						#if defined(DEBUG_OBJECT_NAME)
							materialShaderDescriptorSetLayoutBindingName = Logger::format("Material [], binding []", materialShaderIndex, descriptorSetLayoutBindingIndex);

							imageDescriptor.name = materialShaderDescriptorSetLayoutBindingName;
							imageViewDescriptor.name = materialShaderDescriptorSetLayoutBindingName;
						#endif

						materialShaderImages[imageIndex] = createDynamicImage(imageDescriptor);
						materialShaderImageViews[imageIndex] = materialShaderImages[imageIndex]->createView(imageViewDescriptor);

						imageMemoryBarriers[imageIndex] = {
							.image = materialShaderImages[imageIndex],
							.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
							.newAccess = Access::TRANSFER_WRITE,
						};
						descriptorImages[imageIndex] = {
							.imageView = materialShaderImageViews[imageIndex],
							.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
							.sampler = materialShaderImageSampler,
						};
						writeDescriptorSets[writeDescriptorSetIndex] = {
							.descriptorSet = materialShaderUserDescriptorSet,
							.bindingIndex = descriptorSetLayoutBindingIndex + 1,
							.descriptorImages = descriptorImages[imageIndex],
						};

						imageIndex += 1;
						writeDescriptorSetIndex += 1;

						break;
					}
					default: {
						ENGINE_ERROR_UNSUPPORTED();

						break;
					}
				}

				descriptorSetLayoutBindingIndex += 1;
			}

			materialShaderIndex += 1;
		}
	}

	/// A material shader is a compute shader module written by the user. It contains 2 descriptor sets:
	/// - The first descriptor set (MaterialBase) is automatically set by the renderer. It contains everything needed to access the visibility of a pixel,
	/// as well as additional buffers to extract the triangle, cluster, geometry and instance data from this visibility.
	/// - The second ("user") descriptor is authored by the user. It contains the necessary data for the shader to produce the desired output.
	/// As of now, the bindings in this descriptor must be of type COMBINED_IMAGE_SAMPLER.
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
	void VisibilityRenderer::createMaterialObjects() {
		const ref<Device> device = getDevice();
		const uint32 materialShaderCount = static_cast<uint32>(scene->materialShaders.count());

		materialShaderUserDescriptorSetLayouts = sequence<ref<DescriptorSetLayout>>(materialShaderCount);
		materialShaderUserDescriptorSets = sequence<ref<DescriptorSet>>(materialShaderCount);
		materialShaderPipelineLayouts = sequence<ref<PipelineLayout>>(materialShaderCount);
		materialShaderPipelines = sequence<ref<ComputePipeline>>(materialShaderCount);

		uint32 imageCount = 0;
		uint32 imageAssetCount = 0;
		uint64 imageStagingBufferSize = 0;

		initializeMaterialShaders(imageCount, imageAssetCount, imageStagingBufferSize);

		materialShaderUserDescriptorSets = device->allocateDescriptorSets(materialShaderUserDescriptorSetLayouts);

		if (imageCount == 0) {
			// TODO(renderer/visibility): Material offset write descriptor sets must still be created.
			ENGINE_ERROR_UNIMPLEMENTED();

			return;
		}

		DEBUG_ASSERT(imageAssetCount > 0 && imageStagingBufferSize > 0);

		materialShaderImages = sequence<ref<Image>>(imageCount);
		materialShaderImageViews = sequence<ref<ImageView>>(imageCount);

		sequence<ImageMemoryBarrier> imageMemoryBarriers(imageCount);
		sequence<BaseBufferView> materialOffsetViews(materialShaderCount);
		sequence<DescriptorImage> descriptorImages(imageCount);
		sequence<WriteDescriptorSet> writeDescriptorSets(materialOffsetViews.count() + descriptorImages.count());

		initializeMaterialImages(imageMemoryBarriers, materialOffsetViews, descriptorImages, writeDescriptorSets);

		AssetLibrary* assetLibrary = getLayer()->getApplication()->getAssetLibrary();

		const ref<Buffer> imageStagingBuffer = createDynamicBuffer({
			.size = imageStagingBufferSize,
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Image staging",
			#endif
		});
		const BufferView<uint8> imageStagingView = imageStagingBuffer->createView<uint8>(0, imageStagingBuffer->getSize());

		uint8* imageMappedRange = imageStagingView.map();
		uint32 materialShaderIndex = 0;
		uint32 imageIndex = 0;
		uint64 imageMappedRangeOffset = 0;

		const ref<Queue> graphicsQueue = device->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<CommandEncoder> commandEncoder = graphicsQueue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Copy material shader images",
			#endif
		});

		commandEncoder->begin();
		commandEncoder->insertMemoryBarriers({}, imageMemoryBarriers);

		while (materialShaderIndex < materialShaderCount) {
			const MaterialShader& materialShader = scene->materialShaders[materialShaderIndex];
			const ref<ShaderModuleAsset> materialShaderModuleAsset = materialShader.shaderModuleAsset;
			const std::vector<ref<MaterialAsset>>& materialAssets = scene->materialAssets[materialShaderIndex];

			sequence<BufferImageCopyRegion> copyRegions(materialAssets.size());
			uint32 descriptorSetLayoutBindingIndex = 0;

			while (descriptorSetLayoutBindingIndex < materialShaderModuleAsset->bindings.count()) {
				const DescriptorType descriptorType = materialShaderModuleAsset->bindings[descriptorSetLayoutBindingIndex].descriptorType;

				switch (descriptorType) {
					case DescriptorType::COMBINED_IMAGE_SAMPLER: {
						const ref<Image> image = materialShaderImages[imageIndex];

						uint32 materialIndex = 0;

						while (materialIndex < materialAssets.size()) {
							const ref<MaterialAsset> materialAsset = materialAssets[materialIndex];
							const ref<ImageAsset> imageAsset = assetLibrary->getAsset<ImageAsset>(materialAsset->descriptorImageAssetIdentifiers[descriptorSetLayoutBindingIndex]);
							const Extent3D extent = imageAsset->getExtent();
							const uint64 imageAssetSize = extent.x * extent.y * extent.z * imageAsset->getLayerCount() * getByteSize(ImageFormat::R8G8B8A8_UNORM);

							switch (imageAsset->getFormat()) {
								case ImageFormat::R8G8B8_UNORM: {
									convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(imageAsset->getData(), &imageMappedRange[imageMappedRangeOffset], imageAssetSize);

									break;
								}
								case ImageFormat::R8G8B8A8_UNORM: {
									std::copy(imageAsset->getData().begin(), imageAsset->getData().end(), &imageMappedRange[imageMappedRangeOffset]);

									break;
								}
								default: {
									ENGINE_ERROR_UNSUPPORTED();

									break;
								}
							}

							copyRegions[materialIndex] = {
								.bufferOffset = imageMappedRangeOffset,
								.imageOffsetX = 0,
								.imageOffsetY = 0,
								.imageOffsetZ = 0,
								.imageExtent = imageAsset->getExtent(),
								.imageMipLevel = 0,
								.imageLayerOffset = materialIndex,
								.imageLayerCount = 1,
							};

							imageMappedRangeOffset += imageAssetSize;
							materialIndex += 1;
						}

						commandEncoder->copyBufferToImage(imageStagingBuffer, image, copyRegions);

						imageMemoryBarriers[imageIndex] = {
							.image = image,
							.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
							.newAccess = Access::SHADER_READ,
						};

						imageIndex += 1;

						break;
					}
					default: {
						ENGINE_ERROR_UNSUPPORTED();

						break;
					}
				}

				descriptorSetLayoutBindingIndex += 1;
			}

			materialShaderIndex += 1;
		}

		imageStagingBuffer->unmap();

		commandEncoder->insertMemoryBarriers({}, imageMemoryBarriers);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		graphicsQueue->submit(commandBuffer);
		graphicsQueue->waitIdle();

		device->updateDescriptorSets(writeDescriptorSets);
	}
}