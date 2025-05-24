#include "pch.hpp"

#include "Backend/Vulkan/VulkanBuffer.hpp"
#include "Backend/Vulkan/VulkanCommandBuffer.hpp"
#include "Backend/Vulkan/VulkanCommandEncoder.hpp"
#include "Backend/Vulkan/VulkanComputePipeline.hpp"
#include "Backend/Vulkan/VulkanImage.hpp"
#include "Backend/Vulkan/VulkanRenderPass.hpp"
#include "Backend/Vulkan/VulkanRenderPipeline.hpp"

#include "Backend/Vulkan/CommandBufferUsage.hpp"
#include "Backend/Vulkan/ImageLayout.hpp"
#include "Backend/Vulkan/IndexType.hpp"
#include "Backend/Vulkan/PipelineStage.hpp"

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	VulkanCommandEncoder::VulkanCommandEncoder(const VulkanDevice& device, VkCommandPool commandPool, const CommandBufferDescriptor& descriptor) :
		device(device.object),
		commandPool(commandPool),
		commandBufferUsageFlag(getVkCommandBufferUsageFlag(descriptor.usage)),
		limits(std::dynamic_pointer_cast<VulkanPhysicalDevice>(device.getPhysicalDevice())->getLimits())
	{
		PROFILE_SCOPE(FUNC_SIG);

		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = nullptr;
		commandBufferAllocateInfo.commandPool = commandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;

		const VkResult result = vkAllocateCommandBuffers(device.object, &commandBufferAllocateInfo, &commandBuffer);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan command buffer (code: []).", result);
	}

	VulkanCommandEncoder::~VulkanCommandEncoder() {
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
		DEBUG_VALIDATE_DESCRIPTOR(descriptor);

		const VulkanSwapchain& swapchain = *std::static_pointer_cast<VulkanSwapchain>(descriptor.swapchain);

		fchain<VkClearValue> clearValues(descriptor.clearValues.size());

		for (uint64 clearValueIndex = 0; clearValueIndex < clearValues.count; clearValueIndex += 1) {
			clearValues[clearValueIndex].color.float32[0] = descriptor.clearValues[clearValueIndex].color[0];
			clearValues[clearValueIndex].color.float32[1] = descriptor.clearValues[clearValueIndex].color[1];
			clearValues[clearValueIndex].color.float32[2] = descriptor.clearValues[clearValueIndex].color[2];
			clearValues[clearValueIndex].color.float32[3] = descriptor.clearValues[clearValueIndex].color[3];
			clearValues[clearValueIndex].depthStencil.depth = descriptor.clearValues[clearValueIndex].depth;
			clearValues[clearValueIndex].depthStencil.stencil = descriptor.clearValues[clearValueIndex].stencil;
		}

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.renderPass = std::static_pointer_cast<VulkanRenderPass>(descriptor.renderPass)->object;
		renderPassBeginInfo.framebuffer = swapchain.framebuffers[descriptor.imageIndex];
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = swapchain.getExtentWidth();
		renderPassBeginInfo.renderArea.extent.height = swapchain.getExtentHeight();
		renderPassBeginInfo.clearValueCount = static_cast<uint32>(clearValues.count);
		renderPassBeginInfo.pClearValues = &clearValues[0];

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanCommandEncoder::bindComputePipeline(const ref<ComputePipeline>& computePipeline) {
		this->pipeline = std::dynamic_pointer_cast<VulkanPipeline>(computePipeline);

		vkCmdBindPipeline(commandBuffer, this->pipeline->bindPoint, this->pipeline->object);
	}

	void VulkanCommandEncoder::bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) {
		this->pipeline = std::dynamic_pointer_cast<VulkanPipeline>(renderPipeline);

		vkCmdBindPipeline(commandBuffer, this->pipeline->bindPoint, this->pipeline->object);
	}

	void VulkanCommandEncoder::bindAllDescriptorSets() {
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

	void VulkanCommandEncoder::bindIndexBuffer(const ref<Buffer>& indexBuffer, uint64 offset, uint8 indexTypeSize) {
		vkCmdBindIndexBuffer(
			commandBuffer,
			std::static_pointer_cast<VulkanBuffer>(indexBuffer)->object,
			offset,
			getVkIndexType(indexTypeSize)
		);
	}

	void VulkanCommandEncoder::bindVertexBuffer(uint32 bindingIndex, const ref<Buffer>& vertexBuffer, uint64 offset, uint32 stride) {
		vkCmdBindVertexBuffers(
			commandBuffer,
			bindingIndex,
			1,
			&std::static_pointer_cast<VulkanBuffer>(vertexBuffer)->object,
			&offset
		);
	}

	void VulkanCommandEncoder::copyBufferToBuffer(const ref<Buffer>& sourceBuffer, const ref<Buffer>& destinationBuffer, uint64 sourceOffset, uint64 destinationOffset, uint64 size) {
		ENGINE_ASSERT(size > 0, "Vulkan: The size for a buffer copy operation must be non-zero.");

		VkBufferCopy region{};
		region.srcOffset = sourceOffset;
		region.dstOffset = destinationOffset;
		region.size = size;

		vkCmdCopyBuffer(
			commandBuffer,
			std::static_pointer_cast<VulkanBuffer>(sourceBuffer)->object,
			std::static_pointer_cast<VulkanBuffer>(destinationBuffer)->object,
			1,
			&region
		);
	}

	void VulkanCommandEncoder::copyBufferToImage(const ref<Buffer>& sourceBuffer, const ref<Image>& destinationImage, uint32 width, uint32 height, uint32 depth) {
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
			std::static_pointer_cast<VulkanBuffer>(sourceBuffer)->object,
			std::static_pointer_cast<VulkanImage>(destinationImage)->object,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&bufferImageCopy
		);
	}

	void VulkanCommandEncoder::dispatchGroups(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) {
		ENGINE_ASSERT(groupCountX <= limits.maxComputeWorkgroupCount[0], "Group count in the X axis must not exceed [].", limits.maxComputeWorkgroupCount[0]);
		ENGINE_ASSERT(groupCountY <= limits.maxComputeWorkgroupCount[1], "Group count in the Y axis must not exceed [].", limits.maxComputeWorkgroupCount[1]);
		ENGINE_ASSERT(groupCountZ <= limits.maxComputeWorkgroupCount[2], "Group count in the Z axis must not exceed [].", limits.maxComputeWorkgroupCount[2]);

		vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
	}

	void VulkanCommandEncoder::setImageMemoryBarrier(const ref<Image>& image, ImageLayout oldLayout, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) {
		VkImageMemoryBarrier imageMemoryBarrier{};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.pNext = nullptr;

		switch (oldLayout) {
			case ImageLayout::UNDEFINED:
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_NONE;

				break;
			case ImageLayout::TRANSFER_SRC_OPTIMAL:
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

				break;
			case ImageLayout::TRANSFER_DST_OPTIMAL:
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				break;
			case ImageLayout::SHADER_READ_ONLY_OPTIMAL:
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;

				break;
			default:
				break;
		}

		switch (newLayout) {
			case ImageLayout::TRANSFER_SRC_OPTIMAL:
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

				break;
			case ImageLayout::TRANSFER_DST_OPTIMAL:
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

				break;
			case ImageLayout::SHADER_READ_ONLY_OPTIMAL:
				imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;

				break;
			default:
				break;
		}

		imageMemoryBarrier.oldLayout = getVkImageLayout(oldLayout);
		imageMemoryBarrier.newLayout = getVkImageLayout(newLayout);
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.image = std::static_pointer_cast<VulkanImage>(image)->object;
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

	void VulkanCommandEncoder::setLineWidth(float32 lineWidth) {
		vkCmdSetLineWidth(commandBuffer, lineWidth);
	}

	void VulkanCommandEncoder::setScissor(sint32 xOffset, sint32 yOffset, uint32 width, uint32 height) {
		VkRect2D scissor{};
		scissor.offset.x = xOffset;
		scissor.offset.y = yOffset;
		scissor.extent.width = width;
		scissor.extent.height = height;

		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void VulkanCommandEncoder::setViewport(const vec2<float32>& origin, const vec2<float32>& size, float32 minDepth, float32 maxDepth) {
		VkViewport viewport{};
		viewport.x = origin[0];
		viewport.y = origin[1];
		viewport.width = size[0];
		viewport.height = size[1];
		viewport.minDepth = minDepth;
		viewport.maxDepth = maxDepth;

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	}

	void VulkanCommandEncoder::draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset) {
		vkCmdDraw(commandBuffer, vertexCount, instanceCount, vertexOffset, 0);
	}

	void VulkanCommandEncoder::drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset) {
		vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, indexOffset, 0, 0);
	}

	void VulkanCommandEncoder::endRenderPass() {
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