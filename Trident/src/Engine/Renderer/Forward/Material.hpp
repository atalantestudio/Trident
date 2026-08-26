// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	void ForwardRenderer::createMaterialObjects(
		uint32 materialShaderIndex,
		PipelineLayoutDescriptor& pipelineLayoutDescriptor,
		GraphicsPipelineDescriptor& pipelineDescriptor,
		AssetLibrary& assetLibrary,
		sequence<ref<DescriptorSetLayout>>& uniqueMaterialDescriptorSetLayouts,
		sequence<ref<DescriptorSetLayout>>& materialDescriptorSetLayouts,
		uint32& writeDescriptorSetCount
	) {
		const ref<Scene3DAsset> sceneAsset = scene->getAsset();
		const MaterialShader& materialShader = sceneAsset->materialShaders[materialShaderIndex];
		const ref<ShaderModuleAsset> shaderModuleAsset = materialShader.shaderModuleAsset;

		#if defined(DEBUG_OBJECT_NAME)
			const sequence<char8> materialName = Logger::format("Material []", materialShaderIndex);
			const sequence<char8> materialFragmentName = Logger::format("Material fragment []", materialShaderIndex);
		#endif

		ENGINE_ASSERT(shaderModuleAsset->descriptorSetBindingRanges.count() == 1, "Material shaders must have a single descriptor set.");

		const range<uint32, uint32> bindingRange = shaderModuleAsset->descriptorSetBindingRanges[0];

		ref<DescriptorSetLayout> descriptorSetLayout = createDescriptorSetLayout({
			.bindings = view<DescriptorSetLayoutBinding>(&shaderModuleAsset->bindings[bindingRange.offset], bindingRange.count),
			#if defined(DEBUG_OBJECT_NAME)
				.name = materialName,
			#endif
		});

		uniqueMaterialDescriptorSetLayouts[materialShaderIndex] = descriptorSetLayout;

		writeDescriptorSetCount += materialShader.materialRange.count * bindingRange.count;

		pipelineLayoutDescriptor.descriptorSetLayouts[pipelineLayoutDescriptor.descriptorSetLayouts.count() - 1] = descriptorSetLayout;

		#if defined(DEBUG_OBJECT_NAME)
			pipelineLayoutDescriptor.name = materialName;
		#endif

		ref<PipelineLayout> pipelineLayout = createPipelineLayout(pipelineLayoutDescriptor);

		materialPipelineLayouts[materialShaderIndex] = pipelineLayout;

		pipelineDescriptor.pipelineLayout = pipelineLayout;
		pipelineDescriptor.fragmentShaderModule = loadInternalShaderModule(
			{
				.stage = materialShader.shaderModuleAsset.stageFlags,
				.entryPoint = materialShader.shaderModuleAsset.entryPoint,
				#if defined(DEBUG_OBJECT_NAME)
					.name = materialFragmentName,
				#endif
			},
			materialShader.shaderModuleAsset.name
		);

		#if defined(DEBUG_OBJECT_NAME)
			pipelineDescriptor.name = materialName;
		#endif

		materialPipelines[materialShaderIndex] = createGraphicsPipeline(pipelineDescriptor);

		uint32 materialIndex = 0;

		while (materialIndex < materialShader.materialRange.count) {
			Material& material = sceneAsset->materials[materialShader.materialRange.offset + materialIndex];

			for (const uint32 imageAssetIdentifier : material.asset->descriptorImageAssetIdentifiers) {
				assetLibrary.loadCachedAsset<ImageAsset>(imageAssetIdentifier);
			}

			materialDescriptorSetLayouts[materialShader.materialRange.offset + materialIndex] = descriptorSetLayout;

			materialIndex += 1;
		}
	}

	ref<Image> ForwardRenderer::createMaterialImage(ref<ImageAsset> imageAsset, ImageDescriptor& imageDescriptor) const {
		imageDescriptor.format = imageAsset->getFormat();

		if (imageDescriptor.format == ImageFormat::R8G8B8_UNORM) {
			imageDescriptor.format = ImageFormat::R8G8B8A8_UNORM;
		}

		imageDescriptor.extent = imageAsset->getExtent();
		imageDescriptor.mipLevelCount = imageAsset->getMipLevelCount();
		imageDescriptor.layerCount = imageAsset->getLayerCount();

		return createDynamicImage(imageDescriptor);
	}

	void ForwardRenderer::createMaterials() {
		const ref<Device> device = getDevice();
		const ref<Scene3DAsset> sceneAsset = scene->getAsset();
		const ColorBlendAttachment colorBlendAttachments[] = {
			{
				.blend = false,
			},
		};

		materialPipelineLayouts = sequence<ref<PipelineLayout>>(sceneAsset->materialShaders.count());
		materialPipelines = sequence<ref<GraphicsPipeline>>(sceneAsset->materialShaders.count());

		sequence<ref<DescriptorSetLayout>> uniqueMaterialDescriptorSetLayouts(sceneAsset->materialShaders.count());
		sequence<ref<DescriptorSetLayout>> materialDescriptorSetLayouts(sceneAsset->materials.count());

		AssetLibrary assetLibrary(Filesystem::getProjectRootDirectory());
		assetLibrary.loadAssetMap();

		uint32 materialShaderIndex = 0;
		uint32 writeDescriptorSetCount = 0;

		// Create static material objects.
		{
			PipelineLayoutDescriptor materialPipelineLayoutDescriptor = {
				.descriptorSetLayouts = {sceneDescriptorSetLayout, lightmapAtlasDescriptorSetLayout, nullptr},
			};

			VertexInputBinding vertexInputBindings[3] = {
				VertexInputBinding(InputRate::VERTEX),
				VertexInputBinding(InputRate::VERTEX),
				VertexInputBinding(InputRate::INSTANCE),
			};

			GraphicsPipelineDescriptor materialPipelineDescriptor = getStaticPipelineDescriptor(vertexInputBindings);
			materialPipelineDescriptor.colorBlendState.attachments = colorBlendAttachments;

			while (materialShaderIndex < sceneAsset->dynamicMaterialShaderOffset) {
				createMaterialObjects(materialShaderIndex, materialPipelineLayoutDescriptor, materialPipelineDescriptor, assetLibrary, uniqueMaterialDescriptorSetLayouts, materialDescriptorSetLayouts, writeDescriptorSetCount);

				materialShaderIndex += 1;
			}
		}

		// Create dynamic material objects.
		{
			PipelineLayoutDescriptor materialPipelineLayoutDescriptor = {
				.descriptorSetLayouts = {sceneDescriptorSetLayout, nullptr},
			};

			VertexInputBinding vertexInputBindings[2] = {
				VertexInputBinding(InputRate::VERTEX),
				VertexInputBinding(InputRate::INSTANCE),
			};

			GraphicsPipelineDescriptor materialPipelineDescriptor = getDynamicPipelineDescriptor(vertexInputBindings);
			materialPipelineDescriptor.colorBlendState.attachments = colorBlendAttachments;

			while (materialShaderIndex < sceneAsset->materialShaders.count()) {
				createMaterialObjects(materialShaderIndex, materialPipelineLayoutDescriptor, materialPipelineDescriptor, assetLibrary, uniqueMaterialDescriptorSetLayouts, materialDescriptorSetLayouts, writeDescriptorSetCount);

				materialShaderIndex += 1;
			}
		}

		materialDescriptorSets = device->allocateDescriptorSets(materialDescriptorSetLayouts);

		const view<ImageAsset> imageAssets = assetLibrary.getLoadedAssets<ImageAsset>();
		const uint32 imageCount = static_cast<uint32>(imageAssets.count());

		if (imageCount == 0) {
			return;
		}

		materialImages = sequence<ref<Image>>(imageCount);
		materialImageViews = sequence<ref<ImageView>>(imageCount);

		sequence<DescriptorImage> descriptorImages(imageCount);
		sequence<ImageMemoryBarrier> imageMemoryBarriers(imageCount);
		ImageDescriptor imageDescriptor = {
			.type = ImageType::TYPE_2D,
			.sampleCount = 1,
			.usage = ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL,
		};

		#if defined(DEBUG_OBJECT_NAME)
			sequence<char8> materialName;
			sequence<char8> materialViewName;
		#endif

		uint32 imageIndex = 0;
		uint64 imageStagingBufferSize = 0;

		while (imageIndex < imageCount) {
			const ref<ImageAsset> imageAsset = imageAssets[imageIndex];

			#if defined(DEBUG_OBJECT_NAME)
				materialName = Logger::format("Material []", imageIndex);
				materialViewName = Logger::format("Material view []", imageIndex);

				imageDescriptor.name = materialName;
			#endif

			ref<Image> image = createMaterialImage(imageAsset, imageDescriptor);
			ref<ImageView> imageView = image->createView({
				.mipLevelOffset = 0,
				.mipLevelCount = image->getMipLevelCount(),
				.layerOffset = 0,
				.layerCount = image->getLayerCount(),
				#if defined(DEBUG_OBJECT_NAME)
					.name = materialViewName,
				#endif
			});

			materialImages[imageIndex] = image;
			materialImageViews[imageIndex] = imageView;
			descriptorImages[imageIndex] = {
				.imageView = imageView,
				.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
				.sampler = materialSampler,
			};
			imageMemoryBarriers[imageIndex] = {
				.image = image,
				.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
				.newAccess = Access::TRANSFER_WRITE,
			};

			imageStagingBufferSize = ceil(imageStagingBufferSize, static_cast<uint64>(getByteSize(image->getFormat())));
			imageStagingBufferSize += image->getSize();
			imageIndex += 1;
		}

		const ref<Buffer> imageStagingBuffer = createDynamicBuffer({
			.size = imageStagingBufferSize,
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Material image staging",
			#endif
		});

		const BufferView<uint8> imageStagingView = imageStagingBuffer->createView<uint8>(0, imageStagingBuffer->getSize());

		uint8* imageMappedRange = imageStagingView.map();
		uint64 imageMappedRangeOffset = 0;

		const ref<Queue> queue = device->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Initialize material images",
			#endif
		});

		commandEncoder->begin();
		commandEncoder->insertMemoryBarriers({}, imageMemoryBarriers);

		imageIndex = 0;

		while (imageIndex < imageCount) {
			const ref<Image> image = materialImages[imageIndex];
			const ref<ImageView> imageView = materialImageViews[imageIndex];
			const ref<ImageAsset> imageAsset = imageAssets[imageIndex];
			const view<uint8> data = imageAsset->getData();

			switch (imageAsset->getFormat()) {
				case ImageFormat::R8G8B8_UNORM: {
					convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(data, &imageMappedRange[imageMappedRangeOffset], image->getSize());

					break;
				}
				default: {
					std::copy(data.begin(), data.end(), &imageMappedRange[imageMappedRangeOffset]);

					break;
				}
			}

			const BufferImageCopyRegion imageCopyRegion = {
				.bufferOffset = imageMappedRangeOffset,
				.imageOffsetX = 0,
				.imageOffsetY = 0,
				.imageOffsetZ = 0,
				.imageExtent = imageAsset->getExtent(),
				.imageMipLevel = imageView->getMipLevelOffset(),
				.imageLayerOffset = imageView->getLayerOffset(),
				.imageLayerCount = imageView->getLayerCount(),
			};

			commandEncoder->copyBufferToImage(imageStagingBuffer, image, imageCopyRegion);

			ImageMemoryBarrier& imageMemoryBarrier = imageMemoryBarriers[imageIndex];
			imageMemoryBarrier.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
			imageMemoryBarrier.newAccess = Access::SHADER_READ;

			imageStagingBufferSize = ceil(imageStagingBufferSize, static_cast<uint64>(getByteSize(image->getFormat())));
			imageMappedRangeOffset += image->getSize();

			imageIndex += 1;
		}

		imageStagingBuffer->unmap();

		commandEncoder->insertMemoryBarriers({}, imageMemoryBarriers);

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();

		sequence<WriteDescriptorSet> writeDescriptorSets(writeDescriptorSetCount);
		uint32 materialIndex = 0;
		uint32 bindingOffset = 0;

		while (materialIndex < sceneAsset->materials.count()) {
			const Material& material = sceneAsset->materials[materialIndex];

			uint32 bindingIndex = 0;

			while (bindingIndex < material.asset->descriptorImageAssetIdentifiers.count()) {
				const ref<DescriptorSet> descriptorSet = materialDescriptorSets[materialIndex];
				const uint32 imageAssetIdentifier = material.asset->descriptorImageAssetIdentifiers[bindingIndex];
				const uint32 imageIndex = assetLibrary.getLoadedAssetIndex(imageAssetIdentifier);

				WriteDescriptorSet& writeDescriptorSet = writeDescriptorSets[bindingOffset + bindingIndex];
				writeDescriptorSet.descriptorSet = descriptorSet;
				writeDescriptorSet.bindingIndex = bindingIndex;
				writeDescriptorSet.descriptorImages = descriptorImages[imageIndex];

				bindingIndex += 1;
			}

			bindingOffset += bindingIndex;
			materialIndex += 1;
		}

		device->updateDescriptorSets(writeDescriptorSets);
	}
}