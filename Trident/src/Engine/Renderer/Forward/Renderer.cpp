// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Renderer/Forward/CameraBuffer.hpp"
#include "Engine/Renderer/Forward/ColorImage.hpp"
#include "Engine/Renderer/Forward/DebugContactManifoldBuffer.hpp"
#include "Engine/Renderer/Forward/DebugContactManifoldPipeline.hpp"
#include "Engine/Renderer/Forward/DebugContactManifoldPipelineLayout.hpp"
#include "Engine/Renderer/Forward/DebugHullWireframeBuffer.hpp"
#include "Engine/Renderer/Forward/DebugHullWireframePipeline.hpp"
#include "Engine/Renderer/Forward/DebugHullWireframePipelineLayout.hpp"
#include "Engine/Renderer/Forward/DepthStencilImage.hpp"
#include "Engine/Renderer/Forward/InstanceBuffer.hpp"
#include "Engine/Renderer/Forward/LightmapAtlasImage.hpp"
#include "Engine/Renderer/Forward/LightmapSampler.hpp"
#include "Engine/Renderer/Forward/MaterialSampler.hpp"
#include "Engine/Renderer/Forward/Renderer.hpp"
#include "Engine/Renderer/Forward/RenderPass.hpp"
#include "Engine/Renderer/Forward/Static.hpp"
#include "Engine/Renderer/Forward/StaticDescriptorSetLayout.hpp"
#include "Engine/Renderer/Forward/StaticMaterialPipeline.hpp"
#include "Engine/Renderer/Forward/StaticMaterialPipelineLayout.hpp"
#include "Engine/Renderer/Forward/VertexDescriptorSetLayout.hpp"
#include "Engine/Renderer/Forward/VertexBuffer.hpp"

namespace Trident {
	void ForwardRenderer::createMaterialObjects() {
		// TODO: Use a larger extent, e.g. 4096 * 4096 * 1.
		static constexpr Extent3D IMAGE_EXTENT = {1024, 1024, 1};

		const uint32 materialShaderCount = scene->getMaterialShaderCount();

		// Each material shader has N bindings. 1 image asset is specified per binding for each material shader reference.
		sequence<sequence<sequence<ref<ImageAsset>>>> materialImageAssets(materialShaderCount);

		materialDescriptorSetLayouts = sequence<ref<DescriptorSetLayout>>(materialShaderCount);
		materialDescriptorSets = sequence<ref<DescriptorSet>>(materialShaderCount);
		materialPipelineLayouts = sequence<ref<PipelineLayout>>(materialShaderCount);
		materialPipelines = sequence<ref<GraphicsPipeline>>(materialShaderCount);
		materialImages = sequence<sequence<ref<Image>>>(materialShaderCount);

		PipelineLayoutDescriptor materialPipelineLayoutDescriptor{};

		VertexInputBinding vertexInputBinding(InputRate::VERTEX);
		vertexInputBinding.registerAttribute<svec3<uint32>>();

		VertexInputBinding instanceInputBinding(InputRate::INSTANCE);
		instanceInputBinding.registerAttribute<mat4x4<float32>>();
		instanceInputBinding.registerAttribute<mat3x3<float32>>();

		ShaderModuleDescriptor materialVertexShaderModuleDescriptor{};
		materialVertexShaderModuleDescriptor.stage = ShaderStage::VERTEX;
		materialVertexShaderModuleDescriptor.entryPoint = "main";

		#if defined(DEBUG_OBJECT_NAME)
			materialVertexShaderModuleDescriptor.name = "Material vertex";
		#endif

		ShaderModuleDescriptor materialFragmentShaderModuleDescriptor{};
		materialFragmentShaderModuleDescriptor.stage = ShaderStage::FRAGMENT;
		materialFragmentShaderModuleDescriptor.entryPoint = "main";

		const sequence<ColorBlendAttachment> colorBlendAttachments = {
			{
				.blend = false,
			},
		};

		GraphicsPipelineDescriptor materialPipelineDescriptor{};
		materialPipelineDescriptor.vertexInputBindings = {vertexInputBinding, instanceInputBinding};
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
		materialPipelineDescriptor.depthStencilState.compareOperation = CompareOperation::GREATER;
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

		#if defined(DEBUG_OBJECT_NAME)
			sequence<char8> materialName;
			sequence<char8> materialFragmentShaderName;
		#endif

		uint32 materialShaderIndex = 0;
		uint32 materialBindingCount = 0;
		uint32 imageCopyRegionCount = 0;
		uint64 imageStagingBufferSize = 0;

		while (materialShaderIndex < materialShaderCount) {
			const std::vector<ref<MaterialAsset>>& materialAssets = scene->getMaterialAssets()[materialShaderIndex];

			#if defined(DEBUG_OBJECT_NAME)
				materialName = Logger::format("Material []", materialShaderIndex);
				materialFragmentShaderName = Logger::format("Material [] fragment", materialShaderIndex);

				materialFragmentShaderModuleDescriptor.name = materialFragmentShaderName;
				materialPipelineLayoutDescriptor.name = materialName;
				materialPipelineDescriptor.name = materialName;
			#endif

			sequence<DescriptorSetLayoutBinding> descriptorSetLayoutBindings(materialAssets[0]->descriptorBindingTypes.count());
			uint32 descriptorSetLayoutBindingIndex = 0;

			while (descriptorSetLayoutBindingIndex < descriptorSetLayoutBindings.count()) {
				descriptorSetLayoutBindings[1 + descriptorSetLayoutBindingIndex] = {
					.descriptorType = materialAssets[0]->descriptorBindingTypes[descriptorSetLayoutBindingIndex],
					.descriptorCount = 1,
					.shaderStages = materialAssets[0]->stage,
				};

				descriptorSetLayoutBindingIndex += 1;
			}

			materialDescriptorSetLayouts[materialShaderIndex] = createDescriptorSetLayout(descriptorSetLayoutBindings);

			materialPipelineLayoutDescriptor.descriptorSetLayouts = {vertexDescriptorSetLayout, materialDescriptorSetLayouts[materialShaderIndex]};

			materialPipelineLayouts[materialShaderIndex] = createPipelineLayout(materialPipelineLayoutDescriptor);

			materialPipelineDescriptor.pipelineLayout = materialPipelineLayouts[materialShaderIndex];

			sequence<char8> path(9 + materialAssets[0]->shaderModuleName.count());

			std::copy_n("Material/", 9, &path[0]);
			std::copy_n(&materialAssets[0]->shaderModuleName[0], materialAssets[0]->shaderModuleName.count(), &path[9]);

			materialPipelineDescriptor.fragmentShaderModule = loadInternalShaderModule(materialFragmentShaderModuleDescriptor, path);

			materialPipelines[materialShaderIndex] = createGraphicsPipeline(materialPipelineDescriptor);

			materialImageAssets[materialShaderIndex] = sequence<sequence<ref<ImageAsset>>>(materialAssets[0]->descriptorBindingTypes.count());
			materialImages[materialShaderIndex] = sequence<ref<Image>>(materialAssets[0]->descriptorBindingTypes.count());

			uint32 bindingIndex = 0;

			while (bindingIndex < materialAssets[0]->descriptorBindingTypes.count()) {
				DEBUG_ASSERT(materialAssets[0]->descriptorBindingTypes[bindingIndex] == DescriptorType::COMBINED_IMAGE_SAMPLER);

				materialDescriptorSetLayoutBindingImageDescriptor.layerCount = 1;

				#if defined(DEBUG_OBJECT_NAME)
					materialName = Logger::format("Material [], binding []", materialShaderIndex, bindingIndex);

					materialDescriptorSetLayoutBindingImageDescriptor.name = materialName;
				#endif

				materialImages[materialShaderIndex][bindingIndex] = createDynamicImage(materialDescriptorSetLayoutBindingImageDescriptor);
				materialImageAssets[materialShaderIndex][bindingIndex] = sequence<ref<ImageAsset>>(materialAssets.size());

				uint32 materialAssetIndex = 0;

				while (materialAssetIndex < materialAssets.size()) {
					const ref<MaterialAsset> materialAsset = materialAssets[materialAssetIndex];
					const MaterialDescriptorImage& materialDescriptorImage = materialAsset->descriptorImages[bindingIndex];
					const Extent3D extent = materialDescriptorImage.imageAsset->getExtent();

					materialImageAssets[materialShaderIndex][bindingIndex][materialAssetIndex] = materialDescriptorImage.imageAsset;

					imageStagingBufferSize += extent.width * extent.height * extent.depth * materialDescriptorImage.layerCount * getByteSize(ImageFormat::R8G8B8A8_UNORM);
					imageCopyRegionCount += 1;
					materialAssetIndex += 1;
				}

				bindingIndex += 1;
				materialBindingCount += 1;
			}

			materialShaderIndex += 1;
		}

		materialDescriptorSets = getDevice()->allocateDescriptorSets(materialDescriptorSetLayouts);

		if (imageStagingBufferSize == 0) {
			// TODO: Fix crash in DX12 when binding empty descriptor sets.
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

		#if defined(DEBUG_OBJECT_NAME)
			commandBufferDescriptor.name = "Write layered images";
		#endif

		const ref<Queue> queue = getDevice()->getQueue(QueueFamilyCapability::GRAPHICS);
		const ref<CommandEncoder> commandEncoder = queue->createCommandEncoder(commandBufferDescriptor);

		commandEncoder->begin();

		while (materialShaderIndex < materialShaderCount) {
			const std::vector<ref<MaterialAsset>>& materialAssets = scene->getMaterialAssets()[materialShaderIndex];

			uint32 bindingIndex = 0;

			while (bindingIndex < materialAssets[0]->descriptorBindingTypes.count()) {
				const ref<Image> image = materialImages[materialShaderIndex][bindingIndex];
				const view<DescriptorImage> bindingDescriptorImages(&descriptorImages[materialReferenceBindingImageIndex], materialAssets.size());

				sequence<BufferImageCopyRegion> imageCopyRegions(materialAssets.size());
				uint32 materialIndex = 0;

				while (materialIndex < materialAssets.size()) {
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

					materialReferenceBindingImageViewDescriptor.layerOffset = materialIndex;

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

				commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);
				commandEncoder->copyBufferToImage(imageStagingBuffer, image, imageCopyRegions);

				imageMemoryBarrier = {
					.image = image,
					.newLayout = ImageLayout::SHADER_READ_ONLY_OPTIMAL,
					.newAccess = Access::SHADER_READ,
				};

				commandEncoder->insertMemoryBarriers({}, imageMemoryBarrier);

				bindingIndex += 1;
			}

			materialShaderIndex += 1;
		}

		imageStagingBuffer->unmap();

		const ref<CommandBuffer> commandBuffer = commandEncoder->end();

		queue->submit(commandBuffer);
		queue->waitIdle();

		getDevice()->updateDescriptorSets(writeDescriptorSets);
	}

	void ForwardRenderer::renderStaticObjects(ref<CommandEncoder> commandEncoder) const {
		const ref<Scene3DAsset> sceneAsset = scene->getAsset();
		const view<StaticMaterial3D> staticMaterials = sceneAsset->getStaticMaterials();

		uint32 staticMaterialIndex = 0;

		while (staticMaterialIndex < staticMaterials.count()) {
			const StaticMaterial3D& staticMaterial = staticMaterials[materialIndex];

			// TODO
			commandEncoder->bindGraphicsPipeline(staticMaterialPipeline);
			// commandEncoder->bindGraphicsPipeline(staticMaterialPipelines[staticMaterialIndex]);
			commandEncoder->bindDescriptorSets(staticMaterialPipelineLayout, 0, staticDescriptorSet);
			// commandEncoder->bindDescriptorSets(staticMaterialPipelineLayouts[staticMaterialIndex], 0, staticMaterialDescriptorSets[staticMaterialIndex]);

			commandEncoder->draw(staticMaterial.triangleRange.count * 3, 1, staticMaterial.triangleRange.offset * 3, 0);

			staticMaterialIndex += 1;
		}
	}
}