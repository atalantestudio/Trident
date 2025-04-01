#include "pch.hpp"

#include "Backend/Vulkan/CommandBufferUsage.hpp"
#include "Backend/Vulkan/IndexType.hpp"
#include "Backend/Vulkan/PipelineStage.hpp"
#include "Backend/Vulkan/TextureLayout.hpp"
#include "Backend/Vulkan/VulkanBuffer.hpp"
#include "Backend/Vulkan/VulkanCommandBuffer.hpp"
#include "Backend/Vulkan/VulkanCommandEncoder.hpp"
#include "Backend/Vulkan/VulkanComputePipeline.hpp"
#include "Backend/Vulkan/VulkanRenderPass.hpp"
#include "Backend/Vulkan/VulkanRenderPipeline.hpp"
#include "Backend/Vulkan/VulkanTexture.hpp"

#include "Engine/Application.hpp"

namespace ProjectA {
	VulkanCommandEncoder::VulkanCommandEncoder(VkDevice device, VkCommandPool commandPool, const CommandBufferDescriptor& descriptor) :
		device(device),
		commandPool(commandPool),
		commandBufferUsageFlag(getVkCommandBufferUsageFlag(descriptor.usage))
	{
		PROFILE_SCOPE(FUNC_SIG);

		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = nullptr;
		commandBufferAllocateInfo.commandPool = commandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;

		const VkResult result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan command buffer (code: []).", result);
	}

	VulkanCommandEncoder::~VulkanCommandEncoder() {
		PROFILE_SCOPE(FUNC_SIG);

		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	}

	void VulkanCommandEncoder::begin() const {
		PROFILE_SCOPE(FUNC_SIG);

		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = commandBufferUsageFlag;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		const VkResult result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to begin Vulkan command buffer (code: []).", result);
	}

	void VulkanCommandEncoder::beginRenderPass(const BeginRenderPassDescriptor& descriptor) {
		PROFILE_SCOPE(FUNC_SIG);

		const VulkanSwapchain* swapchain = static_cast<const VulkanSwapchain*>(Application::get().getSwapchain().get());

		VkClearValue clearValue{};

		_mm_store_ps(clearValue.color.float32, descriptor.clearColor.m);

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.renderPass = static_cast<const VulkanRenderPass*>(descriptor.renderPass.get())->object;
		renderPassBeginInfo.framebuffer = swapchain->framebuffers[descriptor.nextImageIndex];
		renderPassBeginInfo.renderArea.offset = {0, 0};
		renderPassBeginInfo.renderArea.extent = swapchain->imageExtent;
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearValue;

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanCommandEncoder::bindPipeline(const ref<Pipeline>& pipeline) {
		PROFILE_SCOPE(FUNC_SIG);

		this->pipeline = std::dynamic_pointer_cast<VulkanPipeline>(pipeline);

		vkCmdBindPipeline(commandBuffer, this->pipeline->bindPoint, this->pipeline->object);
	}

	void VulkanCommandEncoder::bindDescriptorSets(uint32 descriptorSetIndex) const {
		vkCmdBindDescriptorSets(
			commandBuffer,
			pipeline->bindPoint,
			pipeline->layout->object,

			// TODO: Allow custom descriptor set range to be bound.
			0,
			static_cast<uint32>(pipeline->layout->descriptorSets.size()),

			pipeline->layout->descriptorSets.data(),
			0,
			nullptr
		);
	}

	void VulkanCommandEncoder::bindIndexBuffer(const ref<Buffer>& indexBuffer, uint64 offset, IndexType indexType) {
		PROFILE_SCOPE(FUNC_SIG);

		vkCmdBindIndexBuffer(commandBuffer, static_cast<const VulkanBuffer*>(indexBuffer.get())->object, offset, getVkIndexType(indexType));
	}

	void VulkanCommandEncoder::bindVertexBuffer(uint32 bindingIndex, const ref<Buffer>& vertexBuffer, uint32 stride) {
		PROFILE_SCOPE(FUNC_SIG);

		const VkDeviceSize offset = 0;

		vkCmdBindVertexBuffers(commandBuffer, bindingIndex, 1, &static_cast<const VulkanBuffer*>(vertexBuffer.get())->object, &offset);
	}

	void VulkanCommandEncoder::copyBufferToBuffer(const ref<Buffer>& sourceBuffer, const ref<Buffer>& destinationBuffer, uint64 sourceOffset, uint64 destinationOffset, uint64 size) {
		VkBufferCopy bufferCopy{};
		bufferCopy.srcOffset = sourceOffset;
		bufferCopy.dstOffset = destinationOffset;
		bufferCopy.size = size;

		vkCmdCopyBuffer(
			commandBuffer,
			static_cast<const VulkanBuffer*>(sourceBuffer.get())->object,
			static_cast<const VulkanBuffer*>(destinationBuffer.get())->object,
			1,
			&bufferCopy
		);
	}

	void VulkanCommandEncoder::copyBufferToTexture(const ref<Buffer>& sourceBuffer, const ref<Texture>& destinationTexture, uint32 width, uint32 height, uint32 depth) {
		VkBufferImageCopy bufferImageCopy{};
		bufferImageCopy.bufferOffset = 0;
		bufferImageCopy.bufferRowLength = 0;
		bufferImageCopy.bufferImageHeight = 0;
		bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		bufferImageCopy.imageSubresource.mipLevel = 0;
		bufferImageCopy.imageSubresource.baseArrayLayer = 0;
		bufferImageCopy.imageSubresource.layerCount = 1;
		bufferImageCopy.imageOffset.x = 0;
		bufferImageCopy.imageOffset.y = 0;
		bufferImageCopy.imageOffset.z = depth;
		bufferImageCopy.imageExtent.width = width;
		bufferImageCopy.imageExtent.height = height;
		bufferImageCopy.imageExtent.depth = 1;

		vkCmdCopyBufferToImage(
			commandBuffer,
			static_cast<const VulkanBuffer*>(sourceBuffer.get())->object,
			static_cast<const VulkanTexture*>(destinationTexture.get())->object,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&bufferImageCopy
		);
	}

	void VulkanCommandEncoder::dispatchGroups(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) const {
		vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
	}

	void VulkanCommandEncoder::setTextureMemoryBarrier(const ref<Texture>& texture, TextureLayout oldLayout, TextureLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) const {
		VkImageMemoryBarrier imageMemoryBarrier{};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.pNext = nullptr;

		switch (oldLayout) {
			case TextureLayout::UNDEFINED:
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_NONE;

				break;
			case TextureLayout::TRANSFER_SRC_OPTIMAL:
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

				break;
			case TextureLayout::TRANSFER_DST_OPTIMAL:
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				break;
			case TextureLayout::SHADER_READ_ONLY_OPTIMAL:
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;

				break;
			default:
				ENGINE_WARN("Ignored unhandled Vulkan image layout (as old image layout).");

				break;
		}

		switch (newLayout) {
			case TextureLayout::TRANSFER_SRC_OPTIMAL:
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

				break;
			case TextureLayout::TRANSFER_DST_OPTIMAL:
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				break;
			case TextureLayout::SHADER_READ_ONLY_OPTIMAL:
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;

				break;
			default:
				ENGINE_WARN("Ignored unhandled Vulkan image layout (as new image layout).");

				break;
		}

		/*if (oldLayout == TextureLayout::UNDEFINED && newLayout == TextureLayout::TRANSFER_DST_OPTIMAL) {
			imageMemoryBarrier.srcAccessMask = 0;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == TextureLayout::TRANSFER_DST_OPTIMAL && newLayout == TextureLayout::SHADER_READ_ONLY_OPTIMAL) {
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == TextureLayout::UNDEFINED && newLayout == TextureLayout::GENERAL) {
			imageMemoryBarrier.srcAccessMask = 0;
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}*/

		imageMemoryBarrier.oldLayout = getVkImageLayout(oldLayout);
		imageMemoryBarrier.newLayout = getVkImageLayout(newLayout);
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.image = static_cast<const VulkanTexture*>(texture.get())->object;
		imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
		imageMemoryBarrier.subresourceRange.levelCount = 1;
		imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
		imageMemoryBarrier.subresourceRange.layerCount = 1;

		vkCmdPipelineBarrier(
			commandBuffer,
			getVkPipelineStageFlagBits(sourceStage),
			getVkPipelineStageFlagBits(destinationStage),
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&imageMemoryBarrier
		);
	}

	void VulkanCommandEncoder::setViewport(const vec2<float32>& origin, const vec2<float32>& size, float32 minDepth, float32 maxDepth) {
		PROFILE_SCOPE(FUNC_SIG);

		VkViewport viewport{};
		viewport.x = origin[0];
		viewport.y = origin[1];
		viewport.width = size[0];
		viewport.height = size[1];
		viewport.minDepth = minDepth;
		viewport.maxDepth = maxDepth;

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	}

	void VulkanCommandEncoder::setScissor(sint32 xOffset, sint32 yOffset, uint32 width, uint32 height) {
		PROFILE_SCOPE(FUNC_SIG);

		VkRect2D scissor{};
		scissor.offset.x = xOffset;
		scissor.offset.y = yOffset;
		scissor.extent.width = width;
		scissor.extent.height = height;

		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void VulkanCommandEncoder::draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset) {
		PROFILE_SCOPE(FUNC_SIG);

		vkCmdDraw(commandBuffer, vertexCount, instanceCount, vertexOffset, 0);
	}

	void VulkanCommandEncoder::drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset) {
		PROFILE_SCOPE(FUNC_SIG);

		vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, indexOffset, 0, 0);
	}

	void VulkanCommandEncoder::endRenderPass() const {
		PROFILE_SCOPE(FUNC_SIG);

		vkCmdEndRenderPass(commandBuffer);
	}

	ref<CommandBuffer> VulkanCommandEncoder::end() const {
		PROFILE_SCOPE(FUNC_SIG);

		const VkResult result = vkEndCommandBuffer(commandBuffer);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to end Vulkan command buffer (code: []).", result);

		VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		if (commandBufferUsageFlag & VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT) {
			pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}

		return createRef<VulkanCommandBuffer>(commandBuffer, pipelineStageFlags);
	}

	void VulkanCommandEncoder::reset() {
		PROFILE_SCOPE(FUNC_SIG);

		pipeline = nullptr;

		const VkResult result = vkResetCommandBuffer(commandBuffer, 0);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to reset Vulkan command buffer (code: []).", result);
	}
}