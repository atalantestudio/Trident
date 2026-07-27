// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	sequence<ForwardRenderer::MaterialPipelineBase> ForwardRenderer::getMaterialPipelineBases() const {
		sequence<MaterialPipelineBase> bases(2);

		MaterialPipelineBase& staticGeometryBase = bases[0];
		staticGeometryBase.descriptorSetLayouts = {staticGeometryDescriptorSetLayout};
		staticGeometryBase.vertexInputBindings = {getStaticGeometryVertexInputBinding()};
		staticGeometryBase.vertexShaderModule = loadInternalShaderModule(
			{
				.stage = ShaderStage::VERTEX,
				.entryPoint = "main",
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Static geometry vertex",
				#endif
			},
			"Forward/StaticGeometry/StaticGeometry.vert"
		);

		VertexInputBinding forwardVertexInputBinding(InputRate::VERTEX);
		forwardVertexInputBinding.registerAttribute<svec3<uint32>>();

		VertexInputBinding forwardInstanceInputBinding(InputRate::INSTANCE);
		forwardInstanceInputBinding.registerAttribute<mat4x4<float32>>();
		forwardInstanceInputBinding.registerAttribute<mat3x3<float32>>();

		MaterialPipelineBase& forwardBase = bases[1];
		forwardBase.descriptorSetLayouts = {forwardDescriptorSetLayout};
		forwardBase.vertexInputBindings = {forwardVertexInputBinding, forwardInstanceInputBinding};
		forwardBase.vertexShaderModule = loadInternalShaderModule(
			{
				.stage = ShaderStage::VERTEX,
				.entryPoint = "main",
				#if defined(DEBUG_OBJECT_NAME)
					.name = "Forward vertex",
				#endif
			},
			"Forward/Forward.vert"
		);

		return bases;
	}

	void ForwardRenderer::createMaterialPipelines() {
		const ref<Scene3DAsset> sceneAsset = scene->getAsset();
		const ref<Device> device = getDevice();
		const uint32 materialShaderCount = static_cast<uint32>(sceneAsset->materialShaders.count());

		const sequence<MaterialPipelineBase> materialPipelineBases = getMaterialPipelineBases();

		sequence<sequence<sequence<MaterialDescriptorImage>>> materialDescriptorImages(materialShaderCount);
		sequence<ref<DescriptorSetLayout>> materialDescriptorSetLayouts(materialShaderCount);

		materialPipelineLayouts = sequence<ref<PipelineLayout>>(materialShaderCount);
		materialPipelines = sequence<ref<GraphicsPipeline>>(materialShaderCount);
		materialImages = sequence<sequence<sequence<ref<Image>>>>(materialShaderCount);

		PipelineLayoutDescriptor materialPipelineLayoutDescriptor{};

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = false,
			},
		};

		GraphicsPipelineDescriptor materialPipelineDescriptor = {
			.pipelineLayout = nullptr,
			.vertexInputBindings = {},
			.vertexShaderModule = nullptr,
			.fragmentShaderModule = nullptr,
			.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST,
			.polygonMode = PolygonMode::FILL,
			.cullMode = CullMode::BACK,
			.frontFace = FrontFace::CLOCKWISE,
			.lineWidth = 1.0f,
			.minSampleShading = 0.0f,
			.colorBlendState = {
				.attachments = colorBlendAttachments,
			},
			.depthStencilState = {
				.depthTest = true,
				.depthWrite = true,
				.compareOperation = CompareOperation::GREATER,
			},
			.dynamicState = DynamicState::SCISSOR | DynamicState::VIEWPORT,
			.renderPass = renderPass,
			.subpassIndex = 0,
		};

		ImageDescriptor materialImageDescriptor = {
			.type = ImageType::TEXTURE_2D,
			.sampleCount = 1,
			.usage = ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
			.memoryProperties = MemoryProperty::DEVICE_LOCAL | MemoryProperty::LAZILY_ALLOCATED,
		};

		#if defined(DEBUG_OBJECT_NAME)
			sequence<char8> materialName;
			sequence<char8> materialFragmentShaderName;
		#endif

		uint32 materialShaderIndex = 0;
		uint32 imageCopyRegionCount = 0;
		uint64 imageStagingBufferSize = 0;

		while (materialShaderIndex < materialShaderCount) {
			const MaterialShader& materialShader = sceneAsset->materialShaders[materialShaderIndex];
			const ref<ShaderModuleAsset> materialShaderModuleAsset = materialShader.shaderModuleAsset;
			const uint32 descriptorCount = static_cast<uint32>(materialShaderModuleAsset->descriptorSetLayoutBindings.count());

			uint32 assetCount = materialShader.materialAssetRange.count;

			// TODO(forward): Get the correct number of materials per shader.
			if (materialShaderIndex > 0) {
				assetCount = max(1u, assetCount);
			}

			const MaterialPipelineBase& materialPipelineBase = materialPipelineBases[materialShaderIndex];

			materialImages[materialShaderIndex] = sequence<sequence<ref<Image>>>(descriptorCount);
			materialDescriptorImages[materialShaderIndex] = sequence<sequence<MaterialDescriptorImage>>(descriptorCount);

			sequence<DescriptorSetLayoutBinding> bindings(descriptorCount);
			uint32 bindingIndex = 0;

			while (bindingIndex < descriptorCount) {
				const DescriptorType descriptorType = materialShaderModuleAsset->descriptorSetLayoutBindings[bindingIndex].descriptorType;

				DEBUG_ASSERT(isImageDescriptorType(descriptorType));

				bindings[bindingIndex] = {
					.descriptorType = descriptorType,
					.descriptorCount = 1,
					.shaderStages = materialShaderModuleAsset->stage,
				};

				#if defined(DEBUG_OBJECT_NAME)
					materialName = Logger::format("Material [], binding []", materialShaderIndex, bindingIndex);

					materialImageDescriptor.name = materialName;
				#endif

				materialImages[materialShaderIndex][bindingIndex] = sequence<ref<Image>>(assetCount);
				materialDescriptorImages[materialShaderIndex][bindingIndex] = sequence<MaterialDescriptorImage>(assetCount);

				uint32 assetIndex = 0;

				while (assetIndex < assetCount) {
					uint32 offset = materialShader.materialAssetRange.offset;

					// TODO(forward): Get the correct material asset offset.
					if (materialShaderIndex > 0) {
						offset = sceneAsset->materialShaders[materialShaderIndex - 1].materialAssetRange.offset + sceneAsset->materialShaders[materialShaderIndex - 1].materialAssetRange.count;
					}

					const ref<MaterialAsset> asset = sceneAsset->materialAssets[offset + assetIndex];

					MaterialDescriptorImage materialDescriptorImage;

					// TODO(forward): The behavior of non-material resources should be dependent on the shader used.
					if (materialShaderIndex == 0 && bindingIndex < asset->descriptorOffset) {
						materialDescriptorImage.imageAsset = sceneAsset->getLightmapAtlasImageAsset();
					}
					else {
						materialDescriptorImage = asset->descriptorImages[bindingIndex - asset->descriptorOffset];
					}

					const ref<ImageAsset> imageAsset = materialDescriptorImage.imageAsset;

					ImageFormat format = imageAsset->getFormat();

					if (format == ImageFormat::R8G8B8_UNORM) {
						format = ImageFormat::R8G8B8A8_UNORM;
					}

					const Extent3D extent = imageAsset->getExtent();
					const uint32 mipLevelCount = imageAsset->getMipLevelCount();
					const uint32 layerCount = imageAsset->getLayerCount();

					materialImageDescriptor.format = format;
					materialImageDescriptor.extent = extent;
					materialImageDescriptor.mipLevelCount = mipLevelCount;
					materialImageDescriptor.layerCount = layerCount;

					materialImages[materialShaderIndex][bindingIndex][assetIndex] = createDynamicImage(materialImageDescriptor);
					materialDescriptorImages[materialShaderIndex][bindingIndex][assetIndex] = materialDescriptorImage;

					// TODO(forward/materials): Don't count multiple times the same image asset.
					imageStagingBufferSize = ceil(imageStagingBufferSize, static_cast<uint64>(getByteSize(format))) + extent.width * extent.height * extent.depth * layerCount * getByteSize(format);

					assetIndex += 1;
				}

				imageCopyRegionCount += assetCount;
				bindingIndex += 1;
			}

			#if defined(DEBUG_OBJECT_NAME)
				materialName = Logger::format("Material []", materialShaderIndex);
				materialFragmentShaderName = Logger::format("Material [] fragment", materialShaderIndex);

				materialPipelineLayoutDescriptor.name = materialName;
				materialPipelineDescriptor.name = materialName;
			#endif

			materialDescriptorSetLayouts[materialShaderIndex] = createDescriptorSetLayout(bindings);

			materialPipelineLayoutDescriptor.descriptorSetLayouts = sequence<ref<DescriptorSetLayout>>(materialPipelineBase.descriptorSetLayouts.count() + 1);

			std::copy(materialPipelineBase.descriptorSetLayouts.begin(), materialPipelineBase.descriptorSetLayouts.end(), &materialPipelineLayoutDescriptor.descriptorSetLayouts[0]);
			std::copy_n(&materialDescriptorSetLayouts[materialShaderIndex], 1, &materialPipelineLayoutDescriptor.descriptorSetLayouts[materialPipelineBase.descriptorSetLayouts.count()]);

			materialPipelineLayouts[materialShaderIndex] = createPipelineLayout(materialPipelineLayoutDescriptor);

			sequence<char8> materialFragmentShaderModulePath(9 + materialShaderModuleAsset->name.count());

			std::copy_n("Material/", 9, &materialFragmentShaderModulePath[0]);
			std::copy_n(&materialShaderModuleAsset->name[0], materialShaderModuleAsset->name.count(), &materialFragmentShaderModulePath[9]);

			materialPipelineDescriptor.pipelineLayout = materialPipelineLayouts[materialShaderIndex];
			materialPipelineDescriptor.vertexInputBindings = materialPipelineBase.vertexInputBindings;
			materialPipelineDescriptor.vertexShaderModule = materialPipelineBase.vertexShaderModule;
			materialPipelineDescriptor.fragmentShaderModule = loadInternalShaderModule(
				{
					.stage = materialShaderModuleAsset->stage,
					.entryPoint = materialShaderModuleAsset->entryPoint,
					#if defined(DEBUG_OBJECT_NAME)
						.name = materialFragmentShaderName,
					#endif
				},
				materialFragmentShaderModulePath
			);

			materialPipelines[materialShaderIndex] = createGraphicsPipeline(materialPipelineDescriptor);

			materialShaderIndex += 1;
		}

		materialDescriptorSets = device->allocateDescriptorSets(materialDescriptorSetLayouts);

		if (imageStagingBufferSize == 0) {
			// TODO(graphics/dx12/empty-descriptor-set): Fix crash in DX12 when binding empty descriptor sets.
			return;
		}

		const ref<Buffer> imageStagingBuffer = createDynamicBuffer({
			.size = imageStagingBufferSize,
			.usage = BufferUsage::TRANSFER_SRC,
			.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
			.persistent = false,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Image staging",
			#endif
		});
		const ref<BufferView<uint8>> imageStagingView = imageStagingBuffer->createView<uint8>(0, imageStagingBuffer->getSize());

		uint8* imageMappedRange = imageStagingView->map();
		uint64 imageMappedRangeOffset = 0;
		uint32 materialReferenceBindingImageIndex = 0;

		materialReferenceBindingImageViews = sequence<ref<ImageView>>(imageCopyRegionCount);

		sequence<DescriptorImage> descriptorImages(imageCopyRegionCount);
		sequence<WriteDescriptorSet> writeDescriptorSets(imageCopyRegionCount);

		materialShaderIndex = 0;

		ImageViewDescriptor materialReferenceBindingImageViewDescriptor = {
			.mipLevelOffset = 0,
			.mipLevelCount = 1,
			.layerOffset = 0,
			.layerCount = 1,
		};

		const ref<Queue> queue = device->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder({
			.usage = CommandBufferUsage::ONE_TIME_SUBMIT,
			#if defined(DEBUG_OBJECT_NAME)
				.name = "Copy images",
			#endif
		});

		commandEncoder->begin();

		while (materialShaderIndex < materialShaderCount) {
			const MaterialShader& materialShader = sceneAsset->materialShaders[materialShaderIndex];
			const ref<ShaderModuleAsset> materialShaderModuleAsset = materialShader.shaderModuleAsset;

			range<uint32, uint32> materialAssetRange = materialShader.materialAssetRange;

			// TODO(forward): Get the correct material asset range.
			if (materialShaderIndex > 0) {
				materialAssetRange.offset = max(1u, materialAssetRange.offset);
				materialAssetRange.count = max(1u, materialAssetRange.count);
			}

			const view<ref<MaterialAsset>> materialAssets(&sceneAsset->materialAssets[materialAssetRange.offset], materialAssetRange.count);

			uint32 bindingIndex = 0;

			while (bindingIndex < materialShaderModuleAsset->descriptorSetLayoutBindings.count()) {
				const view<DescriptorImage> bindingDescriptorImages(&descriptorImages[materialReferenceBindingImageIndex], materialAssets.count());

				uint32 materialIndex = 0;

				while (materialIndex < materialAssets.count()) {
					const ref<Image> image = materialImages[materialShaderIndex][bindingIndex][materialIndex];
					const MaterialDescriptorImage& materialDescriptorImage = materialDescriptorImages[materialShaderIndex][bindingIndex][materialIndex];
					const ref<ImageAsset> imageAsset = materialDescriptorImage.imageAsset;
					const Extent3D extent = imageAsset->getExtent();

					ImageFormat format = imageAsset->getFormat();

					if (format == ImageFormat::R8G8B8_UNORM) {
						format = ImageFormat::R8G8B8A8_UNORM;
					}

					const uint64 imageStagingSize = extent.width * extent.height * extent.depth * imageAsset->getLayerCount() * getByteSize(format);

					switch (imageAsset->getFormat()) {
						case ImageFormat::R8G8B8_UNORM: {
							convert<ImageFormat::R8G8B8_UNORM, ImageFormat::R8G8B8A8_UNORM>(imageAsset->getData(), &imageMappedRange[imageMappedRangeOffset], imageStagingSize);

							break;
						}
						default: {
							std::copy(imageAsset->getData().begin(), imageAsset->getData().end(), &imageMappedRange[imageMappedRangeOffset]);

							break;
						}
					}

					const BufferImageCopyRegion imageCopyRegion = {
						.bufferOffset = imageMappedRangeOffset,
						.imageOffsetX = 0,
						.imageOffsetY = 0,
						.imageOffsetZ = 0,
						.imageExtent = imageAsset->getExtent(),
						.imageMipLevel = 0,
						.imageLayerOffset = 0,
						.imageLayerCount = 1,
					};

					#if defined(DEBUG_OBJECT_NAME)
						materialName = Logger::format("Material [], binding [], reference []", materialShaderIndex, bindingIndex, materialIndex);

						materialReferenceBindingImageViewDescriptor.name = materialName;
					#endif

					materialReferenceBindingImageViews[materialReferenceBindingImageIndex] = image->createView(materialReferenceBindingImageViewDescriptor);

					descriptorImages[materialReferenceBindingImageIndex] = {
						.imageView = materialReferenceBindingImageViews[materialReferenceBindingImageIndex],
						.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
						.sampler = materialSampler,
					};

					// TODO(forward): The sampler should be dependent on the shader used.
					if (bindingIndex == 0) {
						descriptorImages[materialReferenceBindingImageIndex].sampler = lightmapSampler;
					}

					WriteDescriptorSet& writeDescriptorSet = writeDescriptorSets[materialReferenceBindingImageIndex];
					writeDescriptorSet.descriptorSet = materialDescriptorSets[materialShaderIndex];
					writeDescriptorSet.bindingIndex = bindingIndex;
					writeDescriptorSet.descriptorImages = descriptorImages[materialReferenceBindingImageIndex];

					ImageMemoryBarrier imageMemoryBarrier = {
						.image = image,
						.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
						.newAccess = Access::TRANSFER_WRITE,
					};

					commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);
					commandEncoder->copyBufferToImage(imageStagingBuffer, image, imageCopyRegion);

					imageMemoryBarrier = {
						.image = image,
						.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
						.newAccess = Access::SHADER_READ,
					};

					commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

					imageMappedRangeOffset = ceil(imageMappedRangeOffset, static_cast<uint64>(getByteSize(format))) + imageStagingSize;
					materialIndex += 1;
					materialReferenceBindingImageIndex += 1;
				}

				bindingIndex += 1;
			}

			materialShaderIndex += 1;
		}

		imageStagingBuffer->unmap();

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();

		device->updateDescriptorSets(writeDescriptorSets);
	}
}