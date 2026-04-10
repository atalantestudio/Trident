// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Renderer/Forward/Renderer.hpp"

namespace Trident {
	void ForwardRenderer::createMaterialObjects() {
		// TODO: Use a larger extent, e.g. 4096 * 4096 * 1.
		static constexpr Extent3D IMAGE_EXTENT = {1024, 1024, 1};

		const std::vector<Shader>& materialShaders = scene->getMaterialShaders();

		// Each materialShader has N bindings. 1 image asset is specified per binding for each materialShader reference.
		sequence<sequence<sequence<ref<ImageAsset>>>> materialImageAssets(materialShaders.size());

		materialDescriptorSetLayouts = sequence<ref<DescriptorSetLayout>>(materialShaders.size());
		materialDescriptorSets = sequence<ref<DescriptorSet>>(materialShaders.size());
		materialPipelineLayouts = sequence<ref<PipelineLayout>>(materialShaders.size());
		materialPipelines = sequence<ref<GraphicsPipeline>>(materialShaders.size());
		materialImages = sequence<sequence<ref<Image>>>(materialShaders.size());

		PipelineLayoutDescriptor materialPipelineLayoutDescriptor{};

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<mat4x4<float32>>();
		instanceInputBinding.registerAttribute<mat3x3<float32>>();

		ShaderModuleDescriptor materialVertexShaderModuleDescriptor{};
		materialVertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		materialVertexShaderModuleDescriptor.entryPoint = "main";

		SET_DEBUG_NAME(materialVertexShaderModuleDescriptor, "Material vertex");

		ShaderModuleDescriptor materialFragmentShaderModuleDescriptor{};
		materialFragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		materialFragmentShaderModuleDescriptor.entryPoint = "main";

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = false,
			},
		};

		GraphicsPipelineDescriptor materialPipelineDescriptor{};
		materialPipelineDescriptor.vertexInputBindings = {instanceInputBinding};
		materialPipelineDescriptor.vertexShaderModule = loadInternalShaderModule(materialVertexShaderModuleDescriptor, "Forward/Forward.vert");
		materialPipelineDescriptor.primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		materialPipelineDescriptor.polygonMode = PolygonMode::FILL;
		materialPipelineDescriptor.cullMode = CullMode::NONE;
		materialPipelineDescriptor.frontFace = FrontFace::CLOCKWISE;
		materialPipelineDescriptor.lineWidth = 1.0f;
		materialPipelineDescriptor.minSampleShading = 0.0f;
		materialPipelineDescriptor.colorBlendState.attachments = colorBlendAttachments;
		materialPipelineDescriptor.depthStencilState.depthTest = true;
		materialPipelineDescriptor.depthStencilState.depthWrite = true;
		materialPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::LESS;
		materialPipelineDescriptor.dynamicState = DynamicState::VIEWPORT | DynamicState::SCISSOR;
		materialPipelineDescriptor.renderPass = renderPass;
		materialPipelineDescriptor.subpassIndex = 0;

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

		uint32 materialShaderIndex = 0;
		uint32 bindingIndex = 0;
		uint32 materialBindingCount = 0;
		uint32 materialIndex = 0;
		uint32 imageCopyRegionCount = 0;
		uint64 imageStagingBufferSize = 0;

		while (materialShaderIndex < materialShaders.size()) {
			const Shader& materialShader = materialShaders[materialShaderIndex];
			const std::vector<Material>& materials = scene->getMaterials()[materialShaderIndex];

			#if defined(BUILD_MODE_DEBUG)
				name = Logger::format("Material [] fragment", materialShaderIndex);

				SET_DEBUG_NAME(materialFragmentShaderModuleDescriptor, name);

				name = Logger::format("Material []", materialShaderIndex);

				SET_DEBUG_NAME(materialPipelineLayoutDescriptor, name);
				SET_DEBUG_NAME(materialPipelineDescriptor, name);
			#endif

			materialDescriptorSetLayouts[materialShaderIndex] = createDescriptorSetLayout(materialShader.descriptorSetLayoutBindings);

			materialPipelineLayoutDescriptor.descriptorSetLayouts = {vertexDescriptorSetLayout, materialDescriptorSetLayouts[materialShaderIndex]};

			materialPipelineLayouts[materialShaderIndex] = createPipelineLayout(materialPipelineLayoutDescriptor);

			materialPipelineDescriptor.pipelineLayout = materialPipelineLayouts[materialShaderIndex];
			materialPipelineDescriptor.fragmentShaderModule = loadInternalShaderModule(materialFragmentShaderModuleDescriptor, materialShader.modulePath);

			materialPipelines[materialShaderIndex] = createGraphicsPipeline(materialPipelineDescriptor);

			materialImageAssets[materialShaderIndex] = sequence<sequence<ref<ImageAsset>>>(materialShader.descriptorSetLayoutBindings.count());
			materialImages[materialShaderIndex] = sequence<ref<Image>>(materialShader.descriptorSetLayoutBindings.count());

			bindingIndex = 0;

			while (bindingIndex < materialShader.descriptorSetLayoutBindings.count()) {
				const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = materialShader.descriptorSetLayoutBindings[bindingIndex];

				DEBUG_ASSERT(descriptorSetLayoutBinding.descriptorType == DescriptorType::COMBINED_IMAGE_SAMPLER);
				DEBUG_ASSERT(descriptorSetLayoutBinding.shaderStages == ShaderStage::FRAGMENT);

				materialDescriptorSetLayoutBindingImageDescriptor.layerCount = descriptorSetLayoutBinding.descriptorCount;

				#if defined(BUILD_MODE_DEBUG)
					name = Logger::format("Material [], binding []", materialShaderIndex, bindingIndex);

					SET_DEBUG_NAME(materialDescriptorSetLayoutBindingImageDescriptor, name);
				#endif

				materialImages[materialShaderIndex][bindingIndex] = createImage(materialDescriptorSetLayoutBindingImageDescriptor);
				materialImageAssets[materialShaderIndex][bindingIndex] = sequence<ref<ImageAsset>>(materials.size());

				materialIndex = 0;

				while (materialIndex < materials.size()) {
					const Material& material = materials[materialIndex];
					const ref<ImageAsset> imageAsset = getLayer()->getApplication()->getAssetLibrary()->getAsset<ImageAsset>(material.writeDescriptorSetBindings[bindingIndex].descriptorImages[0].assetPath);

					materialImageAssets[materialShaderIndex][bindingIndex][materialIndex] = imageAsset;

					imageStagingBufferSize += imageAsset->getExtent().width * imageAsset->getExtent().height * imageAsset->getExtent().depth * imageAsset->getLayerCount() * 4;

					imageCopyRegionCount += 1;
					materialIndex += 1;
				}

				bindingIndex += 1;
				materialBindingCount += 1;
			}

			materialShaderIndex += 1;
		}

		materialDescriptorSets = getDevice()->allocateDescriptorSets(materialDescriptorSetLayouts);

		const ref<Buffer<uint8>> imageStagingBuffer = createStagingBuffer<uint8>(imageStagingBufferSize);

		uint8* imageMappedRange = imageStagingBuffer->map(0, imageStagingBuffer->getSize());
		uint64 imageMappedRangeOffset = 0;
		uint32 materialReferenceBindingImageIndex = 0;

		materialReferenceBindingImageViews = sequence<ref<ImageView>>(imageCopyRegionCount);

		sequence<BufferImageCopyRegion> imageCopyRegions(imageCopyRegionCount);
		sequence<DescriptorImage> descriptorImages(imageCopyRegionCount);
		sequence<WriteDescriptorSet> writeDescriptorSets(materialBindingCount);

		materialShaderIndex = 0;

		ImageViewDescriptor materialReferenceBindingImageViewDescriptor{};
		materialReferenceBindingImageViewDescriptor.mipLevelOffset = 0;
		materialReferenceBindingImageViewDescriptor.mipLevelCount = 1;
		materialReferenceBindingImageViewDescriptor.layerCount = 1;

		CommandBufferDescriptor commandBufferDescriptor{};
		commandBufferDescriptor.usage = CommandBufferUsage::ONE_TIME_SUBMIT;

		SET_DEBUG_NAME(commandBufferDescriptor, "Write layered images");

		const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();

		while (materialShaderIndex < materialShaders.size()) {
			const Shader& materialShader = materialShaders[materialShaderIndex];
			const std::vector<Material>& materials = scene->getMaterials()[materialShaderIndex];

			bindingIndex = 0;

			while (bindingIndex < materialShader.descriptorSetLayoutBindings.count()) {
				const ref<Image> image = materialImages[materialShaderIndex][bindingIndex];
				const view<DescriptorImage> bindingDescriptorImages(&descriptorImages[materialReferenceBindingImageIndex], materials.size());

				sequence<BufferImageCopyRegion> imageCopyRegions(materials.size());

				materialIndex = 0;

				while (materialIndex < materials.size()) {
					const ref<ImageAsset> imageAsset = materialImageAssets[materialShaderIndex][bindingIndex][materialIndex];
					const Extent3D imageAssetExtent = imageAsset->getExtent();
					const uint64 imageAssetSize = getChannelCount(ImageFormat::R8G8B8A8_UNORM) * imageAssetExtent.width * imageAssetExtent.height * imageAssetExtent.depth * imageAsset->getLayerCount();

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

					BufferImageCopyRegion& imageCopyRegion = imageCopyRegions[materialIndex];
					imageCopyRegion.bufferOffset = imageMappedRangeOffset;
					imageCopyRegion.imageOffsetX = 0;
					imageCopyRegion.imageOffsetY = 0;
					imageCopyRegion.imageOffsetZ = 0;
					imageCopyRegion.imageExtent = imageAsset->getExtent();
					imageCopyRegion.imageMipLevel = 0;
					imageCopyRegion.imageLayerOffset = materialIndex;
					imageCopyRegion.imageLayerCount = 1;

					materialReferenceBindingImageViewDescriptor.layerOffset = materialIndex;

					#if defined(BUILD_MODE_DEBUG)
						name = Logger::format("Material [], binding [], reference []", materialShaderIndex, bindingIndex, materialIndex);

						SET_DEBUG_NAME(materialReferenceBindingImageViewDescriptor, name);
					#endif

					materialReferenceBindingImageViews[materialReferenceBindingImageIndex] = image->createView(materialReferenceBindingImageViewDescriptor);

					DescriptorImage& descriptorImage = descriptorImages[materialReferenceBindingImageIndex];
					descriptorImage.imageView = materialReferenceBindingImageViews[materialReferenceBindingImageIndex];
					descriptorImage.imageLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;
					descriptorImage.sampler = imageSampler;

					imageMappedRangeOffset += imageAssetSize;
					materialIndex += 1;
					materialReferenceBindingImageIndex += 1;
				}

				WriteDescriptorSet& writeDescriptorSet = writeDescriptorSets[bindingIndex];
				writeDescriptorSet.descriptorSet = materialDescriptorSets[materialShaderIndex];
				writeDescriptorSet.bindingIndex = bindingIndex;
				writeDescriptorSet.descriptorImages = bindingDescriptorImages;

				ImageMemoryBarrier imageMemoryBarrier = {
					.image = image,
					.newLayout = ImageLayout::TRANSFER_DST_OPTIMAL,
					.newAccess = Access::TRANSFER_WRITE,
				};

				commandEncoder->insertMemoryBarriers({}, {imageMemoryBarrier});
				commandEncoder->copyBufferToImage(imageStagingBuffer, image, imageCopyRegions);

				imageMemoryBarrier = {
					.image = image,
					.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
					.newAccess = Access::SHADER_READ,
				};

				commandEncoder->insertMemoryBarriers({}, {imageMemoryBarrier});

				bindingIndex += 1;
			}

			materialShaderIndex += 1;
		}

		imageStagingBuffer->unmap();

		queue->submit({commandEncoder->end()});
		queue->waitIdle();

		getDevice()->updateDescriptorSets(writeDescriptorSets);
	}
}