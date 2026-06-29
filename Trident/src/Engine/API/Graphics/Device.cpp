// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Device.hpp"

namespace Trident {
	#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
		static bool validateSampleCount(uint8 sampleCount) {
			switch (sampleCount) {
				case 1:
				case 2:
				case 4:
				case 8:
				case 16:
				case 32:
				case 64: {
					return true;
				}
				default: {
					return false;
				}
			}
		}
	#endif

	Device::Device(const GraphicsAdapter* adapter, const PhysicalDevice* physicalDevice) :
		adapter(adapter),
		physicalDevice(physicalDevice)
	{}

	Device::~Device() {
		adapter->destroyDeviceImplementation(this);
	}

	AcquireNextImageStatus Device::acquireNextImage(ref<Swapchain> swapchain, ref<Semaphore> semaphore, uint32& imageIndex) const {
		return adapter->acquireNextImageImplementation(this, swapchain.get(), semaphore.get(), imageIndex);
	}

	sequence<ref<DescriptorSet>> Device::allocateDescriptorSets(view<ref<DescriptorSetLayout>> descriptorSetLayouts) {
		return adapter->allocateDescriptorSetsImplementation(this, descriptorSetLayouts);
	}

	ref<BaseBuffer> Device::createStaticBuffer(const BufferDescriptor& descriptor) const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (descriptor.size == 0) {
				ENGINE_ERROR("descriptor.size must not be 0.");

				errorCount += 1;
			}

			if (descriptor.usage & BufferUsage::READ_WRITE_STORAGE && descriptor.memoryProperties & MemoryProperty::HOST_VISIBLE) {
				ENGINE_ERROR("The READ_WRITE_STORAGE buffer usage is mutually exclusive with the HOST_VISIBLE memory property.");

				errorCount += 1;
			}

			if (descriptor.usage & (BufferUsage::READ_ONLY_STORAGE | BufferUsage::READ_WRITE_STORAGE)) {
				const uint64 minStorageBufferOffsetAlignment = physicalDevice->getLimits().minStorageBufferOffsetAlignment;

				if (descriptor.size % minStorageBufferOffsetAlignment > 0) {
					ENGINE_ERROR("descriptor.size must be a multiple of the minStorageBufferOffsetAlignment limit ([]).", minStorageBufferOffsetAlignment);

					errorCount += 1;
				}
			}

			if (descriptor.usage & BufferUsage::UNIFORM) {
				const uint64 minUniformBufferOffsetAlignment = physicalDevice->getLimits().minUniformBufferOffsetAlignment;

				if (descriptor.size % minUniformBufferOffsetAlignment > 0) {
					ENGINE_ERROR("descriptor.size must be a multiple of the minUniformBufferOffsetAlignment limit ([]).", minUniformBufferOffsetAlignment);

					errorCount += 1;
				}
			}

			if (descriptor.name.count() == 0) {
				ENGINE_WARNING("descriptor.name should provide a unique name to help identify this buffer.");
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		return adapter->createBufferImplementation(this, descriptor);
	}

	ref<ComputePipeline> Device::createComputePipeline(const ComputePipelineDescriptor& descriptor) const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (descriptor.pipelineLayout == nullptr) {
				ENGINE_ERROR("descriptor.pipelineLayout must be a valid PipelineLayout object.");

				errorCount += 1;
			}

			if (descriptor.computeShaderModule == nullptr) {
				ENGINE_ERROR("descriptor.computeShaderModule must be a valid ShaderModule object.");

				errorCount += 1;
			}

			if (descriptor.name.count() == 0) {
				ENGINE_WARNING("descriptor.name should provide a unique name to help identify this compute pipeline.");
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		return adapter->createComputePipelineImplementation(this, descriptor);
	}

	ref<DescriptorSetLayout> Device::createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings) const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;
			uint32 descriptorSetLayoutBindingIndex = 0;

			while (descriptorSetLayoutBindingIndex < bindings.count()) {
				const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = bindings[descriptorSetLayoutBindingIndex];

				if (descriptorSetLayoutBinding.descriptorType == DescriptorType::INPUT_ATTACHMENT && descriptorSetLayoutBinding.descriptorCount > 0 && !(descriptorSetLayoutBinding.shaderStages & ShaderStage::FRAGMENT)) {
					ENGINE_ERROR("If descriptorType is INPUT_ATTACHMENT and descriptorCount is not 0, then stageFlags must be FRAGMENT.", descriptorSetLayoutBindingIndex);

					errorCount += 1;
				}

				descriptorSetLayoutBindingIndex += 1;
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		return adapter->createDescriptorSetLayoutImplementation(this, bindings);
	}

	ref<Fence> Device::createFence(const FenceDescriptor& descriptor) const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (descriptor.name.count() == 0) {
				ENGINE_WARNING("descriptor.name should provide a unique name to help identify this fence.");
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		return adapter->createFenceImplementation(this, descriptor);
	}

	ref<Framebuffer> Device::createFramebuffer(const FramebufferDescriptor& descriptor) const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (descriptor.renderPass == nullptr) {
				ENGINE_ERROR("descriptor.renderPass must be a valid RenderPass object.");

				errorCount += 1;
			}
			else {
				// TODO(graphics/validation-layer): The number of image views must be the same as the number of attachments (including resolve), not attachment references.
				/* if (descriptor.imageViews.count != descriptor.renderPass->getAttachmentReferenceCount()) {
					ENGINE_ERROR("descriptor.imageViews.count must be equal to the attachment reference count specified in descriptor.renderPass.");

					errorCount += 1;
				} */
			}

			for (uint32 imageViewIndex = 0; imageViewIndex < descriptor.imageViews.count(); imageViewIndex += 1) {
				const ref<ImageView> imageView = descriptor.imageViews[imageViewIndex];

				if (imageView == nullptr) {
					ENGINE_ERROR("descriptor.imageViews[[]] must be a valid ImageView object.", imageViewIndex);

					errorCount += 1;
				}

				if (descriptor.extent.width > imageView->getImage()->getExtent().width) {
					ENGINE_ERROR("descriptor.imageViews[[]] must have been created with a width greater than or equal to descriptor.extent.width.", imageViewIndex);

					errorCount += 1;
				}

				if (descriptor.extent.height > imageView->getImage()->getExtent().height) {
					ENGINE_ERROR("descriptor.imageViews[[]] must have been created with a height greater than or equal to descriptor.extent.height.", imageViewIndex);

					errorCount += 1;
				}
			}

			if (descriptor.extent.width == 0) {
				ENGINE_ERROR("descriptor.extent.width must be greater than 0.");

				errorCount += 1;
			}

			if (descriptor.extent.height == 0) {
				ENGINE_ERROR("descriptor.extent.height must be greater than 0.");

				errorCount += 1;
			}

			if (descriptor.name.count() == 0) {
				ENGINE_WARNING("descriptor.name should provide a unique name to help identify this framebuffer.");
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		return adapter->createFramebufferImplementation(this, descriptor);
	}

	ref<GraphicsPipeline> Device::createGraphicsPipeline(const GraphicsPipelineDescriptor& descriptor) const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;
			uint32 vertexInputBindingIndex = 0;
			uint32 vertexInputAttributeIndex;
			uint32 colorBlendAttachmentIndex;

			if (descriptor.pipelineLayout == nullptr) {
				ENGINE_ERROR("descriptor.pipelineLayout must be a valid PipelineLayout object.");

				errorCount += 1;
			}

			while (vertexInputBindingIndex < descriptor.vertexInputBindings.count()) {
				const VertexInputBinding& vertexInputBinding = descriptor.vertexInputBindings[vertexInputBindingIndex];

				if (vertexInputBinding.getInputRate() >= InputRate::count) {
					ENGINE_ERROR("descriptor.vertexInputBindings[[]].inputRate must be a valid InputRate value.", vertexInputBindingIndex);

					errorCount += 1;
				}

				const std::vector<VertexInputAttribute>& vertexInputAttributes = vertexInputBinding.getAttributes();

				vertexInputAttributeIndex = 0;

				while (vertexInputAttributeIndex < vertexInputAttributes.size()) {
					const VertexInputAttribute& vertexInputAttribute = vertexInputAttributes[vertexInputAttributeIndex];

					if (vertexInputAttribute.type >= VertexInputAttributeType::count) {
						ENGINE_ERROR("descriptor.vertexInputBindings[[]].attributes[[]].type must be a valid VertexInputAttributeType value.", vertexInputBindingIndex, vertexInputAttributeIndex);

						errorCount += 1;
					}

					vertexInputAttributeIndex += 1;
				}

				vertexInputBindingIndex += 1;
			}

			if (descriptor.vertexShaderModule == nullptr) {
				ENGINE_ERROR("descriptor.vertexShaderModule must be a valid ShaderModule object.");

				errorCount += 1;
			}

			if (descriptor.fragmentShaderModule == nullptr) {
				ENGINE_ERROR("descriptor.fragmentShaderModule must be a valid ShaderModule object.");

				errorCount += 1;
			}

			if (descriptor.primitiveTopology >= PrimitiveTopology::count) {
				ENGINE_ERROR("descriptor.primitiveTopology must be a valid PrimitiveTopology value.");

				errorCount += 1;
			}

			if (descriptor.polygonMode >= PolygonMode::count) {
				ENGINE_ERROR("descriptor.polygonMode must be a valid PolygonMode value.");

				errorCount += 1;
			}

			if (descriptor.cullMode >= CullMode::count) {
				ENGINE_ERROR("descriptor.cullMode must be a valid CullMode value.");

				errorCount += 1;
			}

			if (descriptor.frontFace >= FrontFace::count) {
				ENGINE_ERROR("descriptor.frontFace must be a valid FrontFace value.");

				errorCount += 1;
			}

			if (descriptor.lineWidth <= 0.0f) {
				ENGINE_ERROR("descriptor.lineWidth should be greater than 0.");

				errorCount += 1;
			}

			if (descriptor.minSampleShading < 0.0f || descriptor.minSampleShading > 1.0f) {
				ENGINE_ERROR("descriptor.minSampleShading must be in the range [0, 1].");

				errorCount += 1;
			}

			colorBlendAttachmentIndex = 0;

			while (colorBlendAttachmentIndex < descriptor.colorBlendState.attachments.count()) {
				const ColorBlendAttachment& colorBlendAttachment = descriptor.colorBlendState.attachments[colorBlendAttachmentIndex];

				if (colorBlendAttachment.blend) {
					if (colorBlendAttachment.srcColorBlendFactor >= BlendFactor::count) {
						ENGINE_ERROR("If descriptor.colorBlendState.attachments[[]].blend is true, descriptor.colorBlendState.attachments[[]].srcColorBlendFactor must be a valid BlendFactor value.", colorBlendAttachmentIndex, colorBlendAttachmentIndex);

						errorCount += 1;
					}

					if (colorBlendAttachment.dstColorBlendFactor >= BlendFactor::count) {
						ENGINE_ERROR("If descriptor.colorBlendState.attachments[[]].blend is true, descriptor.colorBlendState.attachments[[]].dstColorBlendFactor must be a valid BlendFactor value.", colorBlendAttachmentIndex, colorBlendAttachmentIndex);

						errorCount += 1;
					}
				}

				colorBlendAttachmentIndex += 1;
			}

			if (descriptor.depthStencilState.depthTest) {
				if (descriptor.depthStencilState.compareOperation >= CompareOperation::count) {
					ENGINE_ERROR("If descriptor.depthStencilState.depthStencil is true, descriptor.depthStencilState.compareOperation must be a valid CompareOperation value.");

					errorCount += 1;
				}
			}

			if (!(descriptor.dynamicState & DynamicState::SCISSOR)) {
				ENGINE_ERROR("descriptor.dynamicState should include DynamicState::SCISSOR.");

				errorCount += 1;
			}

			if (!(descriptor.dynamicState & DynamicState::VIEWPORT)) {
				ENGINE_ERROR("descriptor.dynamicState should include DynamicState::VIEWPORT.");

				errorCount += 1;
			}

			if (descriptor.renderPass == nullptr) {
				ENGINE_ERROR("descriptor.renderPass must be a valid RenderPass object.");

				errorCount += 1;
			}
			else {
				if (descriptor.subpassIndex >= descriptor.renderPass->getSubpassCount()) {
					ENGINE_ERROR("descriptor.subpassIndex must be less than the number of subpasses in descriptor.renderPass.");

					errorCount += 1;
				}
				else {
					const Subpass& subpass = descriptor.renderPass->getSubpass(descriptor.subpassIndex);

					if (descriptor.colorBlendState.attachments.count() != subpass.colorAttachmentReferences.count()) {
						ENGINE_ERROR("descriptor.colorBlendState.attachments must have one entry for each color attachment reference in the subpass.");

						errorCount += 1;
					}

					if (descriptor.depthStencilState.depthTest && subpass.depthStencilAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
						ENGINE_WARNING("descriptor.depthStencilState.depthTest is true but the subpass does not contain a depth stencil attachment.");
					}
				}
			}

			if (descriptor.name.count() == 0) {
				ENGINE_WARNING("descriptor.name should provide a unique name to help identify this graphics pipeline.");
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		return adapter->createGraphicsPipelineImplementation(this, descriptor);
	}

	ref<Image> Device::createStaticImage(const ImageDescriptor& descriptor) const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			const PhysicalDeviceLimits& limits = physicalDevice->getLimits();

			uint32 errorCount = 0;

			if (descriptor.type >= ImageType::count) {
				ENGINE_ERROR("descriptor.type must be a valid ImageType value.");

				errorCount += 1;
			}

			if (descriptor.format >= ImageFormat::count) {
				ENGINE_ERROR("descriptor.format must be a valid ImageFormat value.");

				errorCount += 1;
			}

			if (descriptor.format == ImageFormat::R8G8B8_UNORM) {
				ENGINE_ERROR("descriptor.format must not be R8G8B8_UNORM. This format only exists for asset storage.");

				errorCount += 1;
			}

			if (descriptor.extent.width == 0) {
				ENGINE_ERROR("descriptor.extent.width must not be 0.");

				errorCount += 1;
			}

			if (descriptor.extent.width > max(limits.maxImageDimension1D, max(limits.maxImageDimension2D, limits.maxImageDimension3D))) {
				ENGINE_ERROR("descriptor.extent.width must be less than or equal to the maximum dimension for 1D, 2D or 3D images.");

				errorCount += 1;
			}

			if (descriptor.extent.height == 0) {
				ENGINE_ERROR("descriptor.extent.height must not be 0.");

				errorCount += 1;
			}

			if (descriptor.extent.height > max(limits.maxImageDimension2D, limits.maxImageDimension3D)) {
				ENGINE_ERROR("descriptor.extent.height must be less than or equal to the maximum dimension for 2D or 3D images.");

				errorCount += 1;
			}

			if (descriptor.extent.depth == 0) {
				ENGINE_ERROR("descriptor.extent.depth must not be 0.");

				errorCount += 1;
			}

			if (descriptor.extent.depth > limits.maxImageDimension3D) {
				ENGINE_ERROR("descriptor.extent.depth must be less than or equal to the maximum dimension for 3D images.");

				errorCount += 1;
			}

			if (descriptor.mipLevelCount == 0) {
				ENGINE_ERROR("descriptor.mipLevelCount must not be 0.");

				errorCount += 1;
			}

			const uint32 maxMipLevelCount = calculateMaxMipLevelCount(descriptor.extent);

			if (descriptor.mipLevelCount > maxMipLevelCount) {
				ENGINE_ERROR("descriptor.mipLevelCount must be less than or equal to the maximum mip level count for this size ([]).", maxMipLevelCount);

				errorCount += 1;
			}

			if (descriptor.layerCount == 0) {
				ENGINE_ERROR("descriptor.layerCount must not be 0.");

				errorCount += 1;
			}

			if (descriptor.layerCount > limits.maxImageLayerCount) {
				ENGINE_ERROR("descriptor.layerCount must be less than or equal to the maximum layer count.");

				errorCount += 1;
			}

			if (!validateSampleCount(descriptor.sampleCount)) {
				ENGINE_ERROR("descriptor.sampleCount must either be 1 or a power of 2 (up to 64).");

				errorCount += 1;
			}

			if (descriptor.name.count() == 0) {
				ENGINE_WARNING("descriptor.name should provide a unique name to help identify this image.");
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		return adapter->createImageImplementation(this, descriptor);
	}

	ref<Image> Device::createImage(const ImageDescriptor& descriptor) const {
		const ref<Image> image = createStaticImage(descriptor);

		memoryAllocator->suballocateDynamic(image);

		return image;
	}

	ref<PipelineLayout> Device::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;
			uint32 descriptorSetLayoutIndex = 0;
			uint32 descriptorSetLayoutBindingIndex;

			while (descriptorSetLayoutIndex < descriptor.descriptorSetLayouts.count()) {
				const ref<DescriptorSetLayout> descriptorSetLayout = descriptor.descriptorSetLayouts[descriptorSetLayoutIndex];

				descriptorSetLayoutBindingIndex = 0;

				while (descriptorSetLayoutBindingIndex < descriptorSetLayout->bindings.count()) {
					const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = descriptorSetLayout->bindings[descriptorSetLayoutBindingIndex];

					if (descriptorSetLayoutBinding.descriptorType >= DescriptorType::count) {
						ENGINE_ERROR("descriptor.descriptorSetLayouts[[]]->bindings[[]].descriptorType must be a valid DescriptorType value.", descriptorSetLayoutIndex, descriptorSetLayoutBindingIndex);

						errorCount += 1;
					}

					if (descriptorSetLayoutBinding.descriptorCount == 0) {
						ENGINE_ERROR("descriptor.descriptorSetLayouts[[]]->bindings[[]].descriptorCount must not be 0.", descriptorSetLayoutIndex, descriptorSetLayoutBindingIndex);

						errorCount += 1;
					}

					descriptorSetLayoutBindingIndex += 1;
				}

				descriptorSetLayoutIndex += 1;
			}

			if (descriptor.name.count() == 0) {
				ENGINE_WARNING("descriptor.name should provide a unique name to help identify this pipeline layout.");
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		return adapter->createPipelineLayoutImplementation(this, descriptor);
	}

	ref<RenderPass> Device::createRenderPass(ref<Surface> surface, const RenderPassDescriptor& descriptor) const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (descriptor.attachments.count() == 0) {
				ENGINE_ERROR("descriptor.attachments must not be empty.");

				errorCount += 1;
			}

			uint32 attachmentIndex = 0;

			while (attachmentIndex < descriptor.attachments.count()) {
				const Attachment& attachment = descriptor.attachments[attachmentIndex];

				if (attachment.format >= ImageFormat::count) {
					ENGINE_ERROR("descriptor.attachments[[]].format must be a valid ImageFormat value.", attachmentIndex);

					errorCount += 1;
				}

				if (!validateSampleCount(attachment.sampleCount)) {
					ENGINE_ERROR("descriptor.attachments[[]].sampleCount must either be 1 or a power of 2 (up to 64).", attachmentIndex);

					errorCount += 1;
				}

				if (attachment.loadOperation >= LoadOperation::count) {
					ENGINE_ERROR("descriptor.attachments[[]].loadOperation must be a valid LoadOperation value.", attachmentIndex);

					errorCount += 1;
				}

				if (attachment.storeOperation >= StoreOperation::count) {
					ENGINE_ERROR("descriptor.attachments[[]].storeOperation must be a valid StoreOperation value.", attachmentIndex);

					errorCount += 1;
				}

				if (attachment.finalLayout >= ImageLayout::count) {
					ENGINE_ERROR("descriptor.attachments[[]].finalLayout must be a valid ImageLayout value.", attachmentIndex);

					errorCount += 1;
				}

				attachmentIndex += 1;
			}

			if (descriptor.subpasses.count() == 0) {
				ENGINE_ERROR("descriptor.subpasses must not be empty.");

				errorCount += 1;
			}

			uint32 subpassIndex = 0;

			while (subpassIndex < descriptor.subpasses.count()) {
				const Subpass& subpass = descriptor.subpasses[subpassIndex];

				if (subpass.colorAttachmentReferences.count() == 0) {
					ENGINE_ERROR("descriptor.subpasses[[]].colorAttachmentReferences must not be empty.", subpassIndex);

					errorCount += 1;
				}

				uint32 colorAttachmentReferenceIndex = 0;
				bool anyUsedColorAttachment = false;

				while (colorAttachmentReferenceIndex < subpass.colorAttachmentReferences.count()) {
					const AttachmentReference& colorAttachmentReference = subpass.colorAttachmentReferences[colorAttachmentReferenceIndex];

					if (colorAttachmentReference.index != AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
						anyUsedColorAttachment = true;

						if (colorAttachmentReference.layout >= ImageLayout::count) {
							ENGINE_ERROR("descriptor.subpasses[[]].colorAttachmentReferences[[]].layout must be a valid ImageLayout value.", subpassIndex, colorAttachmentReferenceIndex);

							errorCount += 1;
						}
					}

					colorAttachmentReferenceIndex += 1;
				}

				if (!anyUsedColorAttachment) {
					ENGINE_ERROR("descriptor.subpasses[[]].colorAttachmentReferences must contain at least one used attachment.", subpassIndex);

					errorCount += 1;
				}

				if (subpass.depthStencilAttachmentReference.index != AttachmentReference::UNUSED_ATTACHMENT_INDEX && subpass.depthStencilAttachmentReference.layout >= ImageLayout::count) {
					ENGINE_ERROR("descriptor.subpasses[[]].depthStencilAttachmentReference.layout must be a valid ImageLayout value.", subpassIndex);

					errorCount += 1;
				}

				uint32 inputAttachmentReferenceIndex = 0;

				while (inputAttachmentReferenceIndex < subpass.inputAttachmentReferences.count()) {
					const AttachmentReference& inputAttachmentReference = subpass.inputAttachmentReferences[inputAttachmentReferenceIndex];

					if (inputAttachmentReference.index != AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
						if (inputAttachmentReference.layout >= ImageLayout::count) {
							ENGINE_ERROR("descriptor.subpasses[[]].inputAttachmentReferences[[]].layout must be a valid ImageLayout value.", subpassIndex, inputAttachmentReferenceIndex);

							errorCount += 1;
						}
					}

					inputAttachmentReferenceIndex += 1;
				}

				subpassIndex += 1;
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		return adapter->createRenderPassImplementation(this, surface, descriptor);
	}

	ref<Sampler> Device::createSampler(const SamplerDescriptor& descriptor) const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (descriptor.magFilter >= SamplerFilter::count) {
				ENGINE_ERROR("descriptor.magFilter must be a valid SamplerFilter value.");

				errorCount += 1;
			}

			if (descriptor.minFilter >= SamplerFilter::count) {
				ENGINE_ERROR("descriptor.minFilter must be a valid SamplerFilter value.");

				errorCount += 1;
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		return adapter->createSamplerImplementation(this, descriptor);
	}

	ref<Semaphore> Device::createSemaphore() const {
		return adapter->createSemaphoreImplementation(this);
	}

	ref<ShaderModule> Device::createShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (descriptor.entryPoint.count() == 0) {
				ENGINE_ERROR("descriptor.entryPoint must not be empty.");

				errorCount += 1;
			}

			if (descriptor.name.count() == 0) {
				ENGINE_WARNING("descriptor.name should provide a unique name to help identify this shader module.");
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		return adapter->createShaderModuleFromAssetImplementation(this, descriptor, asset);
	}

	ref<ShaderModule> Device::createShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code) const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 errorCount = 0;

			if (descriptor.entryPoint.count() == 0) {
				ENGINE_ERROR("descriptor.entryPoint must not be empty.");

				errorCount += 1;
			}

			if (descriptor.name.count() == 0) {
				ENGINE_WARNING("descriptor.name should provide a unique name to help identify this shader module.");
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		return adapter->createShaderModuleFromCodeImplementation(this, descriptor, code);
	}

	ref<Swapchain> Device::createSwapchain(const SwapchainDescriptor& descriptor) const {
		return adapter->createSwapchainImplementation(this, descriptor);
	}

	ref<MemoryAllocator> Device::getMemoryAllocator() const {
		return memoryAllocator;
	}

	ref<PhysicalDevice> Device::getPhysicalDevice() const {
		return createRef<PhysicalDevice>(*physicalDevice);
	}

	ref<Queue> Device::getQueue(QueueFamilyCapability queueFamilyCapability) const {
		return adapter->getQueueImplementation(this, queueFamilyCapability);
	}

	void Device::resetFence(ref<Fence> fence) const {
		adapter->resetFenceImplementation(this, fence.get());
	}

	void Device::updateDescriptorSets(view<WriteDescriptorSet> writeDescriptorSets) const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			const uint64 minStorageBufferOffsetAlignment = physicalDevice->getLimits().minStorageBufferOffsetAlignment;

			uint32 errorCount = 0;
			uint32 writeDescriptorSetIndex = 0;

			while (writeDescriptorSetIndex < writeDescriptorSets.count()) {
				const WriteDescriptorSet& writeDescriptorSet = writeDescriptorSets[writeDescriptorSetIndex];

				if (writeDescriptorSet.descriptorSet == nullptr) {
					ENGINE_ERROR("writeDescriptorSets[[]].descriptorSet must be a valid DescriptorSet object.", writeDescriptorSetIndex);

					errorCount += 1;
				}

				uint32 descriptorBufferIndex = 0;

				while (descriptorBufferIndex < writeDescriptorSet.descriptorBuffers.count()) {
					const DescriptorBuffer& descriptorBuffer = writeDescriptorSet.descriptorBuffers[descriptorBufferIndex];
					const ref<BaseBuffer> buffer = descriptorBuffer.buffer;

					if (buffer == nullptr) {
						ENGINE_ERROR("writeDescriptorSets[[]].descriptorBuffers[[]].buffer must be a valid Buffer object.", writeDescriptorSetIndex, descriptorBufferIndex);

						errorCount += 1;
					}
					else {
						// TODO(graphics/device/update-descriptor-sets)
						// This validation breaks the Visibility sample, which offsets the Material Offset buffer from 4 bytes for the prefix sum.
						if (buffer->getUsage() & (BufferUsage::READ_ONLY_STORAGE | BufferUsage::READ_WRITE_STORAGE)) {
							if (descriptorBuffer.offset % minStorageBufferOffsetAlignment > 0) {
								ENGINE_ERROR("writeDescriptorSets[[]].descriptorBuffers[[]].offset must be a multiple of the minStorageBufferOffsetAlignment physical device limit.", writeDescriptorSetIndex, descriptorBufferIndex);

								errorCount += 1;
							}
						}
					}

					descriptorBufferIndex += 1;
				}

				writeDescriptorSetIndex += 1;
			}

			ENGINE_ASSERT(errorCount == 0, "Invalid argument(s) ([] errors).", errorCount);
		#endif

		adapter->updateDescriptorSetsImplementation(this, writeDescriptorSets);
	}

	void Device::waitForFence(ref<Fence> fence) const {
		adapter->waitForFenceImplementation(this, fence.get());
	}

	void Device::waitIdle() const {
		adapter->deviceWaitIdleImplementation(this);
	}
}