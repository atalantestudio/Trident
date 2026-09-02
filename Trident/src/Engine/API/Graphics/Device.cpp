// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Graphics/Descriptor/FenceDescriptor.hpp"
#include "Engine/API/Graphics/Descriptor/ShaderModuleDescriptor.hpp"
#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/PhysicalDevice.hpp"
#include "Engine/API/Graphics/Queue.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace Trident {
	#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
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

		static void validateShaderModuleDescriptor(const ShaderModuleDescriptor& descriptor) {
			Validator validator;

			if (descriptor.stage == 0) {
				VALIDATION_ERROR("descriptor.stage must be a valid ShaderStage flag combination.");
			}

			if (countSetBits(static_cast<uint32>(descriptor.stage)) != 1) {
				VALIDATION_ERROR("descriptor.stage must not be a combination of different shader stages.");
			}

			if (descriptor.entryPoint.count() == 0) {
				VALIDATION_ERROR("descriptor.entryPoint must not be empty.");
			}

			if (descriptor.name.count() == 0) {
				VALIDATION_WARNING("descriptor.name should provide a unique name to help identify this shader module.");
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
		return adapter->acquireNextImageImplementation(this, swapchain, semaphore, imageIndex);
	}

	sequence<ref<DescriptorSet>> Device::allocateDescriptorSets(view<ref<DescriptorSetLayout>> descriptorSetLayouts) {
		return adapter->allocateDescriptorSetsImplementation(this, descriptorSetLayouts);
	}

	ref<Buffer> Device::createDynamicBuffer(const BufferDescriptor& descriptor) const {
		const ref<Buffer> buffer = createStaticBuffer(descriptor);

		memoryAllocator->suballocateDynamic(buffer);

		return buffer;
	}

	ref<Buffer> Device::createStaticBuffer(const BufferDescriptor& descriptor) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (descriptor.size == 0) {
					VALIDATION_ERROR("descriptor.size must not be 0.");
				}

				if (descriptor.usage & BufferUsage::READ_WRITE_STORAGE && descriptor.memoryProperties & MemoryProperty::HOST_VISIBLE) {
					VALIDATION_WARNING("READ_WRITE_STORAGE buffers should not have the HOST_VISIBLE memory property.");
				}

				if (descriptor.usage & (BufferUsage::READ_ONLY_STORAGE | BufferUsage::READ_WRITE_STORAGE)) {
					const uint64 minStorageBufferOffsetAlignment = physicalDevice->getLimits().minStorageBufferOffsetAlignment;

					if (descriptor.size % minStorageBufferOffsetAlignment > 0) {
						VALIDATION_ERROR("descriptor.size must be a multiple of the minStorageBufferOffsetAlignment limit ([]).", minStorageBufferOffsetAlignment);
					}
				}

				if (descriptor.usage & BufferUsage::UNIFORM) {
					const uint64 minUniformBufferOffsetAlignment = physicalDevice->getLimits().minUniformBufferOffsetAlignment;

					if (descriptor.size % minUniformBufferOffsetAlignment > 0) {
						VALIDATION_ERROR("descriptor.size must be a multiple of the minUniformBufferOffsetAlignment limit ([]).", minUniformBufferOffsetAlignment);
					}
				}

				if (descriptor.name.count() == 0) {
					VALIDATION_WARNING("descriptor.name should provide a unique name to help identify this buffer.");
				}
			}
		#endif

		return adapter->createBufferImplementation(this, descriptor);
	}

	ref<ComputePipeline> Device::createComputePipeline(const ComputePipelineDescriptor& descriptor) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (descriptor.pipelineLayout == nullptr) {
					VALIDATION_ERROR("descriptor.pipelineLayout must be a valid PipelineLayout object.");
				}

				if (descriptor.computeShaderModule == nullptr) {
					VALIDATION_ERROR("descriptor.computeShaderModule must be a valid ShaderModule object.");
				}

				if (descriptor.name.count() == 0) {
					VALIDATION_WARNING("descriptor.name should provide a unique name to help identify this compute pipeline.");
				}
			}
		#endif

		return adapter->createComputePipelineImplementation(this, descriptor);
	}

	ref<DescriptorSetLayout> Device::createDescriptorSetLayout(const DescriptorSetLayoutDescriptor& descriptor) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;
				uint32 bindingIndex = 0;

				while (bindingIndex < descriptor.bindings.count()) {
					const DescriptorSetLayoutBinding& binding = descriptor.bindings[bindingIndex];

					if (binding.descriptorType == DescriptorType::INPUT_ATTACHMENT && binding.descriptorCount > 0 && !(binding.stageFlags & ShaderStage::FRAGMENT)) {
						VALIDATION_ERROR("For descriptor.bindings[[]], if descriptorType is INPUT_ATTACHMENT and descriptorCount is not 0, then stageFlags must be FRAGMENT.", bindingIndex);
					}

					bindingIndex += 1;
				}

				if (descriptor.name.count() == 0) {
					VALIDATION_WARNING("descriptor.name should provide a unique name to help identify this descriptor set layout.");
				}
			}
		#endif

		return adapter->createDescriptorSetLayoutImplementation(this, descriptor);
	}

	ref<Fence> Device::createFence(const FenceDescriptor& descriptor) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (descriptor.name.count() == 0) {
					VALIDATION_WARNING("descriptor.name should provide a unique name to help identify this fence.");
				}
			}
		#endif

		return adapter->createFenceImplementation(this, descriptor);
	}

	ref<Framebuffer> Device::createFramebuffer(const FramebufferDescriptor& descriptor) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (descriptor.renderPass == nullptr) {
					VALIDATION_ERROR("descriptor.renderPass must be a valid RenderPass object.");
				}
				else {
					// TODO(graphics/validation-layer): The number of image views must be the same as the number of attachments (including resolve), not attachment references.
					/* if (descriptor.imageViews.count != descriptor.renderPass->getAttachmentReferenceCount()) {
						VALIDATION_ERROR("descriptor.imageViews.count must be equal to the attachment reference count specified in descriptor.renderPass.");
					} */
				}

				for (uint32 imageViewIndex = 0; imageViewIndex < descriptor.imageViews.count(); imageViewIndex += 1) {
					const ref<ImageView> imageView = descriptor.imageViews[imageViewIndex];

					if (imageView == nullptr) {
						VALIDATION_ERROR("descriptor.imageViews[[]] must be a valid ImageView object.", imageViewIndex);
					}

					if (descriptor.extent.width > imageView->getImage()->getExtent().width) {
						VALIDATION_ERROR("descriptor.imageViews[[]] must have been created with a width greater than or equal to descriptor.extent.width.", imageViewIndex);
					}

					if (descriptor.extent.height > imageView->getImage()->getExtent().height) {
						VALIDATION_ERROR("descriptor.imageViews[[]] must have been created with a height greater than or equal to descriptor.extent.height.", imageViewIndex);
					}
				}

				if (descriptor.extent.width == 0) {
					VALIDATION_ERROR("descriptor.extent.width must be greater than 0.");
				}

				if (descriptor.extent.height == 0) {
					VALIDATION_ERROR("descriptor.extent.height must be greater than 0.");
				}

				if (descriptor.name.count() == 0) {
					VALIDATION_WARNING("descriptor.name should provide a unique name to help identify this framebuffer.");
				}
			}
		#endif

		return adapter->createFramebufferImplementation(this, descriptor);
	}

	ref<GraphicsPipeline> Device::createGraphicsPipeline(const GraphicsPipelineDescriptor& descriptor) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (descriptor.pipelineLayout == nullptr) {
					VALIDATION_ERROR("descriptor.pipelineLayout must be a valid PipelineLayout object.");
				}

				uint32 vertexInputBindingIndex = 0;

				while (vertexInputBindingIndex < descriptor.vertexInputBindings.count()) {
					const VertexInputBinding& vertexInputBinding = descriptor.vertexInputBindings[vertexInputBindingIndex];

					if (vertexInputBinding.getInputRate() >= InputRate::count) {
						VALIDATION_ERROR("descriptor.vertexInputBindings[[]].inputRate must be a valid InputRate value.", vertexInputBindingIndex);
					}

					const std::vector<VertexInputAttribute>& vertexInputAttributes = vertexInputBinding.getAttributes();

					uint32 vertexInputAttributeIndex = 0;

					while (vertexInputAttributeIndex < vertexInputAttributes.size()) {
						const VertexInputAttribute& vertexInputAttribute = vertexInputAttributes[vertexInputAttributeIndex];

						if (vertexInputAttribute.format >= VertexInputAttributeFormat::count) {
							VALIDATION_ERROR("descriptor.vertexInputBindings[[]].attributes[[]].format must be a valid VertexInputAttributeFormat value.", vertexInputBindingIndex, vertexInputAttributeIndex);
						}

						vertexInputAttributeIndex += 1;
					}

					vertexInputBindingIndex += 1;
				}

				if (descriptor.vertexShaderModule == nullptr) {
					VALIDATION_ERROR("descriptor.vertexShaderModule must be a valid ShaderModule object.");
				}

				if (descriptor.fragmentShaderModule == nullptr && descriptor.colorBlendState.attachments.count() > 0) {
					VALIDATION_ERROR("descriptor.fragmentShaderModule must be a valid ShaderModule object if the pipeline references color attachments.");
				}

				if (descriptor.primitiveTopology >= PrimitiveTopology::count) {
					VALIDATION_ERROR("descriptor.primitiveTopology must be a valid PrimitiveTopology value.");
				}

				if (descriptor.polygonMode >= PolygonMode::count) {
					VALIDATION_ERROR("descriptor.polygonMode must be a valid PolygonMode value.");
				}

				if (descriptor.cullMode >= CullMode::count) {
					VALIDATION_ERROR("descriptor.cullMode must be a valid CullMode value.");
				}

				if (descriptor.frontFace >= FrontFace::count) {
					VALIDATION_ERROR("descriptor.frontFace must be a valid FrontFace value.");
				}

				if (descriptor.lineWidth <= 0.0f) {
					VALIDATION_ERROR("descriptor.lineWidth should be greater than 0.");
				}

				if (descriptor.minSampleShading < 0.0f || descriptor.minSampleShading > 1.0f) {
					VALIDATION_ERROR("descriptor.minSampleShading must be in the range [0, 1].");
				}

				uint32 colorBlendAttachmentIndex = 0;

				while (colorBlendAttachmentIndex < descriptor.colorBlendState.attachments.count()) {
					const ColorBlendAttachment& colorBlendAttachment = descriptor.colorBlendState.attachments[colorBlendAttachmentIndex];

					if (colorBlendAttachment.blend) {
						if (colorBlendAttachment.srcColorBlendFactor >= BlendFactor::count) {
							VALIDATION_ERROR("If descriptor.colorBlendState.attachments[[]].blend is true, descriptor.colorBlendState.attachments[[]].srcColorBlendFactor must be a valid BlendFactor value.", colorBlendAttachmentIndex, colorBlendAttachmentIndex);
						}

						if (colorBlendAttachment.dstColorBlendFactor >= BlendFactor::count) {
							VALIDATION_ERROR("If descriptor.colorBlendState.attachments[[]].blend is true, descriptor.colorBlendState.attachments[[]].dstColorBlendFactor must be a valid BlendFactor value.", colorBlendAttachmentIndex, colorBlendAttachmentIndex);
						}
					}

					colorBlendAttachmentIndex += 1;
				}

				if (descriptor.depthStencilState.depthTest) {
					if (descriptor.depthStencilState.compareOperation >= CompareOperation::count) {
						VALIDATION_ERROR("If descriptor.depthStencilState.depthStencil is true, descriptor.depthStencilState.compareOperation must be a valid CompareOperation value.");
					}
				}

				if (!(descriptor.dynamicState & DynamicState::SCISSOR)) {
					VALIDATION_ERROR("descriptor.dynamicState should include DynamicState::SCISSOR.");
				}

				if (!(descriptor.dynamicState & DynamicState::VIEWPORT)) {
					VALIDATION_ERROR("descriptor.dynamicState should include DynamicState::VIEWPORT.");
				}

				if (descriptor.renderPass == nullptr) {
					VALIDATION_ERROR("descriptor.renderPass must be a valid RenderPass object.");
				}
				else {
					if (descriptor.subpassIndex >= descriptor.renderPass->getSubpassCount()) {
						VALIDATION_ERROR("descriptor.subpassIndex must be less than the number of subpasses in descriptor.renderPass.");
					}
					else {
						const Subpass& subpass = descriptor.renderPass->getSubpass(descriptor.subpassIndex);

						if (descriptor.colorBlendState.attachments.count() != subpass.colorAttachmentReferences.count()) {
							VALIDATION_ERROR("descriptor.colorBlendState.attachments must have one entry for each color attachment reference in the subpass.");
						}

						if (descriptor.depthStencilState.depthTest && subpass.depthStencilAttachmentReference.index == AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
							VALIDATION_WARNING("descriptor.depthStencilState.depthTest is true but the subpass does not contain a depth stencil attachment.");
						}
					}
				}

				if (descriptor.name.count() == 0) {
					VALIDATION_WARNING("descriptor.name should provide a unique name to help identify this graphics pipeline.");
				}
			}
		#endif

		return adapter->createGraphicsPipelineImplementation(this, descriptor);
	}

	ref<Image> Device::createDynamicImage(const ImageDescriptor& descriptor) const {
		const ref<Image> image = createStaticImage(descriptor);

		memoryAllocator->suballocateDynamic(image);

		return image;
	}

	ref<Image> Device::createStaticImage(const ImageDescriptor& descriptor) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (descriptor.type >= ImageType::count) {
					VALIDATION_ERROR("descriptor.type must be a valid ImageType value.");
				}

				if (descriptor.flags & ImageFlags::CUBE_COMPATIBLE) {
					if (descriptor.layerCount != 6) {
						VALIDATION_ERROR("descriptor.layerCount must be 6 if the image has the CUBE_COMPATIBLE flag.");
					}
				}

				if (descriptor.format >= ImageFormat::count) {
					VALIDATION_ERROR("descriptor.format must be a valid ImageFormat value.");
				}

				if (descriptor.format == ImageFormat::R8G8B8_UNORM) {
					VALIDATION_ERROR("descriptor.format must not be R8G8B8_UNORM. This format only exists for asset storage.");
				}

				if (descriptor.extent.width == 0) {
					VALIDATION_ERROR("descriptor.extent.width must not be 0.");
				}

				const PhysicalDeviceLimits& limits = physicalDevice->getLimits();

				if (descriptor.extent.width > max(limits.maxImageDimension1D, max(limits.maxImageDimension2D, limits.maxImageDimension3D))) {
					VALIDATION_ERROR("descriptor.extent.width must be less than or equal to the maximum dimension for 1D, 2D or 3D images.");
				}

				if (descriptor.extent.height == 0) {
					VALIDATION_ERROR("descriptor.extent.height must not be 0.");
				}

				if (descriptor.extent.height > max(limits.maxImageDimension2D, limits.maxImageDimension3D)) {
					VALIDATION_ERROR("descriptor.extent.height must be less than or equal to the maximum dimension for 2D or 3D images.");
				}

				if (descriptor.extent.depth == 0) {
					VALIDATION_ERROR("descriptor.extent.depth must not be 0.");
				}

				if (descriptor.extent.depth > limits.maxImageDimension3D) {
					VALIDATION_ERROR("descriptor.extent.depth must be less than or equal to the maximum dimension for 3D images.");
				}

				if (descriptor.mipLevelCount == 0) {
					VALIDATION_ERROR("descriptor.mipLevelCount must not be 0.");
				}

				const uint32 maxMipLevelCount = calculateMaxMipLevelCount(descriptor.extent);

				if (descriptor.mipLevelCount > maxMipLevelCount) {
					VALIDATION_ERROR("descriptor.mipLevelCount must be less than or equal to the maximum mip level count for this size ([]).", maxMipLevelCount);
				}

				if (descriptor.layerCount == 0) {
					VALIDATION_ERROR("descriptor.layerCount must not be 0.");
				}

				if (descriptor.layerCount > limits.maxImageLayerCount) {
					VALIDATION_ERROR("descriptor.layerCount must be less than or equal to the maximum layer count.");
				}

				if (!validateSampleCount(descriptor.sampleCount)) {
					VALIDATION_ERROR("descriptor.sampleCount must either be 1 or a power of 2 (up to 64).");
				}

				if (descriptor.name.count() == 0) {
					VALIDATION_WARNING("descriptor.name should provide a unique name to help identify this image.");
				}
			}
		#endif

		return adapter->createImageImplementation(this, descriptor);
	}

	ref<PipelineLayout> Device::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;
				uint32 descriptorSetLayoutIndex = 0;

				while (descriptorSetLayoutIndex < descriptor.descriptorSetLayouts.count()) {
					const ref<DescriptorSetLayout> descriptorSetLayout = descriptor.descriptorSetLayouts[descriptorSetLayoutIndex];

					uint32 descriptorSetLayoutBindingIndex = 0;

					while (descriptorSetLayoutBindingIndex < descriptorSetLayout->bindings.count()) {
						const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = descriptorSetLayout->bindings[descriptorSetLayoutBindingIndex];

						if (descriptorSetLayoutBinding.descriptorType >= DescriptorType::count) {
							VALIDATION_ERROR("descriptor.descriptorSetLayouts[[]]->bindings[[]].descriptorType must be a valid DescriptorType value.", descriptorSetLayoutIndex, descriptorSetLayoutBindingIndex);
						}

						if (descriptorSetLayoutBinding.descriptorCount == 0) {
							VALIDATION_ERROR("descriptor.descriptorSetLayouts[[]]->bindings[[]].descriptorCount must not be 0.", descriptorSetLayoutIndex, descriptorSetLayoutBindingIndex);
						}

						descriptorSetLayoutBindingIndex += 1;
					}

					descriptorSetLayoutIndex += 1;
				}

				if (descriptor.name.count() == 0) {
					VALIDATION_WARNING("descriptor.name should provide a unique name to help identify this pipeline layout.");
				}
			}
		#endif

		return adapter->createPipelineLayoutImplementation(this, descriptor);
	}

	ref<RenderPass> Device::createRenderPass(const RenderPassDescriptor& descriptor) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (descriptor.attachments.count() == 0) {
					VALIDATION_ERROR("descriptor.attachments must not be empty.");
				}

				uint32 attachmentIndex = 0;

				while (attachmentIndex < descriptor.attachments.count()) {
					const Attachment& attachment = descriptor.attachments[attachmentIndex];

					if (attachment.format >= ImageFormat::count) {
						VALIDATION_ERROR("descriptor.attachments[[]].format must be a valid ImageFormat value.", attachmentIndex);
					}

					if (!validateSampleCount(attachment.sampleCount)) {
						VALIDATION_ERROR("descriptor.attachments[[]].sampleCount must either be 1 or a power of 2 (up to 64).", attachmentIndex);
					}

					if (attachment.loadOperation >= LoadOperation::count) {
						VALIDATION_ERROR("descriptor.attachments[[]].loadOperation must be a valid LoadOperation value.", attachmentIndex);
					}

					if (attachment.storeOperation >= StoreOperation::count) {
						VALIDATION_ERROR("descriptor.attachments[[]].storeOperation must be a valid StoreOperation value.", attachmentIndex);
					}

					if (attachment.finalLayout >= ImageLayout::count) {
						VALIDATION_ERROR("descriptor.attachments[[]].finalLayout must be a valid ImageLayout value.", attachmentIndex);
					}

					attachmentIndex += 1;
				}

				if (descriptor.subpasses.count() == 0) {
					VALIDATION_ERROR("descriptor.subpasses must not be empty.");
				}

				uint32 subpassIndex = 0;

				while (subpassIndex < descriptor.subpasses.count()) {
					const Subpass& subpass = descriptor.subpasses[subpassIndex];

					uint32 colorAttachmentReferenceIndex = 0;

					while (colorAttachmentReferenceIndex < subpass.colorAttachmentReferences.count()) {
						const AttachmentReference& colorAttachmentReference = subpass.colorAttachmentReferences[colorAttachmentReferenceIndex];

						if (colorAttachmentReference.index != AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
							if (colorAttachmentReference.layout >= ImageLayout::count) {
								VALIDATION_ERROR("descriptor.subpasses[[]].colorAttachmentReferences[[]].layout must be a valid ImageLayout value.", subpassIndex, colorAttachmentReferenceIndex);
							}
						}

						colorAttachmentReferenceIndex += 1;
					}

					if (subpass.depthStencilAttachmentReference.index != AttachmentReference::UNUSED_ATTACHMENT_INDEX && subpass.depthStencilAttachmentReference.layout >= ImageLayout::count) {
						VALIDATION_ERROR("descriptor.subpasses[[]].depthStencilAttachmentReference.layout must be a valid ImageLayout value.", subpassIndex);
					}

					uint32 inputAttachmentReferenceIndex = 0;

					while (inputAttachmentReferenceIndex < subpass.inputAttachmentReferences.count()) {
						const AttachmentReference& inputAttachmentReference = subpass.inputAttachmentReferences[inputAttachmentReferenceIndex];

						if (inputAttachmentReference.index != AttachmentReference::UNUSED_ATTACHMENT_INDEX) {
							if (inputAttachmentReference.layout >= ImageLayout::count) {
								VALIDATION_ERROR("descriptor.subpasses[[]].inputAttachmentReferences[[]].layout must be a valid ImageLayout value.", subpassIndex, inputAttachmentReferenceIndex);
							}
						}

						inputAttachmentReferenceIndex += 1;
					}

					subpassIndex += 1;
				}
			}
		#endif

		return adapter->createRenderPassImplementation(this, descriptor);
	}

	ref<Sampler> Device::createSampler(const SamplerDescriptor& descriptor) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (descriptor.magFilter >= Filter::count) {
					VALIDATION_ERROR("descriptor.magFilter must be a valid Filter value.");
				}

				if (descriptor.minFilter >= Filter::count) {
					VALIDATION_ERROR("descriptor.minFilter must be a valid Filter value.");
				}

				if (descriptor.addressModeU >= AddressMode::count) {
					VALIDATION_ERROR("descriptor.addressModeU must be a valid AddressMode value.");
				}

				if (descriptor.addressModeV >= AddressMode::count) {
					VALIDATION_ERROR("descriptor.addressModeV must be a valid AddressMode value.");
				}

				if (descriptor.addressModeW >= AddressMode::count) {
					VALIDATION_ERROR("descriptor.addressModeW must be a valid AddressMode value.");
				}

				if (descriptor.name.count() == 0) {
					VALIDATION_WARNING("descriptor.name should provide a unique name to help identify this sampler.");
				}
			}
		#endif

		return adapter->createSamplerImplementation(this, descriptor);
	}

	ref<Semaphore> Device::createSemaphore() const {
		return adapter->createSemaphoreImplementation(this);
	}

	ref<ShaderModule> Device::createShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			validateShaderModuleDescriptor(descriptor);
		#endif

		return adapter->createShaderModuleFromAssetImplementation(this, descriptor, asset);
	}

	ref<ShaderModule> Device::createShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			validateShaderModuleDescriptor(descriptor);
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
		return physicalDevice;
	}

	ref<Queue> Device::getQueue(QueueFamilyCapability queueFamilyCapability) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				Validator validator;

				if (queueFamilyCapability >= QueueFamilyCapability::count) {
					VALIDATION_ERROR("queueFamilyCapability must be a valid QueueFamilyCapability value.");
				}
			}
		#endif

		return queues[static_cast<uint32>(queueFamilyCapability)];
	}

	void Device::resetFence(ref<Fence> fence) const {
		adapter->resetFenceImplementation(this, fence);
	}

	void Device::updateDescriptorSets(view<WriteDescriptorSet> writeDescriptorSets) const {
		#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			{
				const uint64 minStorageBufferOffsetAlignment = physicalDevice->getLimits().minStorageBufferOffsetAlignment;

				Validator validator;
				uint32 writeDescriptorSetIndex = 0;

				while (writeDescriptorSetIndex < writeDescriptorSets.count()) {
					const WriteDescriptorSet& writeDescriptorSet = writeDescriptorSets[writeDescriptorSetIndex];

					if (writeDescriptorSet.descriptorSet == nullptr) {
						VALIDATION_ERROR("writeDescriptorSets[[]].descriptorSet must be a valid DescriptorSet object.", writeDescriptorSetIndex);
					}

					// TODO(graphics/device/descriptor-set-layout)
					// Expose a way to get the descriptor set layout from the descriptor set,
					// then validate the number of buffer/image descriptors.

					uint32 descriptorBufferIndex = 0;

					while (descriptorBufferIndex < writeDescriptorSet.descriptorBufferViews.count()) {
						const BaseBufferView& bufferView = writeDescriptorSet.descriptorBufferViews[descriptorBufferIndex];
						const Buffer* buffer = bufferView.getBuffer();

						if (buffer == nullptr) {
							VALIDATION_ERROR("writeDescriptorSets[[]].descriptorBufferViews[[]].buffer must be a valid Buffer object.", writeDescriptorSetIndex, descriptorBufferIndex);
						}
						else {
							// TODO(graphics/device/update-descriptor-sets)
							// This validation breaks the Visibility sample, which offsets the Material Offset buffer from 4 bytes for the prefix sum.
							if (buffer->getUsage() & (BufferUsage::READ_ONLY_STORAGE | BufferUsage::READ_WRITE_STORAGE)) {
								if (bufferView.getOffset() % minStorageBufferOffsetAlignment > 0) {
									VALIDATION_ERROR("writeDescriptorSets[[]].descriptorBufferViews[[]].offset must be a multiple of the minStorageBufferOffsetAlignment physical device limit.", writeDescriptorSetIndex, descriptorBufferIndex);
								}
							}
						}

						descriptorBufferIndex += 1;
					}

					writeDescriptorSetIndex += 1;
				}
			}
		#endif

		adapter->updateDescriptorSetsImplementation(this, writeDescriptorSets);
	}

	void Device::waitForFence(ref<Fence> fence) const {
		adapter->waitForFenceImplementation(this, fence);
	}

	void Device::waitIdle() const {
		adapter->deviceWaitIdleImplementation(this);
	}
}