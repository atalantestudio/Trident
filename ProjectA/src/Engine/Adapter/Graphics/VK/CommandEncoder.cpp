#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Buffer.hpp"
#include "Engine/Adapter/Graphics/VK/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/VK/CommandBufferUsage.hpp"
#include "Engine/Adapter/Graphics/VK/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/VK/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/VK/DescriptorSet.hpp"
#include "Engine/Adapter/Graphics/VK/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/VK/Image.hpp"
#include "Engine/Adapter/Graphics/VK/ImageLayout.hpp"
#include "Engine/Adapter/Graphics/VK/IndexType.hpp"
#include "Engine/Adapter/Graphics/VK/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/VK/PipelineStage.hpp"
#include "Engine/Adapter/Graphics/VK/RenderPass.hpp"
#include "Engine/Adapter/Graphics/VK/RenderPipeline.hpp"

#include "Engine/Renderer/Renderer.hpp"

namespace ProjectA {
	VulkanCommandEncoder::VulkanCommandEncoder(const VulkanDevice& device, VkCommandPool commandPool, const CommandBufferDescriptor& descriptor) :
		device(device.object),
		commandPool(commandPool),
		commandBufferUsageFlag(getVkCommandBufferUsageFlag(descriptor.usage)),
		limits(std::dynamic_pointer_cast<VulkanPhysicalDevice>(device.getPhysicalDevice())->getLimits())
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = nullptr;
		commandBufferAllocateInfo.commandPool = commandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;

		const VkResult result = vkAllocateCommandBuffers(device.object, &commandBufferAllocateInfo, &commandBuffer);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan command buffer ([]).", string_VkResult(result));
	}

	VulkanCommandEncoder::~VulkanCommandEncoder() {
		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	}

	void VulkanCommandEncoder::begin() const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = commandBufferUsageFlag;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		const VkResult result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to begin Vulkan command buffer ([]).", string_VkResult(result));
	}

	void VulkanCommandEncoder::beginRenderPass(const BeginRenderPassDescriptor& descriptor) {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		const VulkanFramebuffer& framebuffer = *std::static_pointer_cast<VulkanFramebuffer>(descriptor.framebuffer);
		const VulkanRenderPass& renderPass = *std::static_pointer_cast<VulkanRenderPass>(framebuffer.getRenderPass());

		sequence<VkClearValue> clearValues(descriptor.clearValues.count());

		// TODO: Refactor clear values.
		for (uint64 clearValueIndex = 0; clearValueIndex < clearValues.count(); clearValueIndex += 1) {
			if (descriptor.clearValues[clearValueIndex].depth != 0.0f || descriptor.clearValues[clearValueIndex].stencil != 0) {
				clearValues[clearValueIndex].depthStencil.depth = descriptor.clearValues[clearValueIndex].depth;
				clearValues[clearValueIndex].depthStencil.stencil = descriptor.clearValues[clearValueIndex].stencil;

				continue;
			}

			clearValues[clearValueIndex].color.float32[0] = descriptor.clearValues[clearValueIndex].color[0];
			clearValues[clearValueIndex].color.float32[1] = descriptor.clearValues[clearValueIndex].color[1];
			clearValues[clearValueIndex].color.float32[2] = descriptor.clearValues[clearValueIndex].color[2];
			clearValues[clearValueIndex].color.float32[3] = descriptor.clearValues[clearValueIndex].color[3];
		}

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.renderPass = renderPass.object;
		renderPassBeginInfo.framebuffer = framebuffer.object;
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent = *pointer_cast<const VkExtent2D*>(&framebuffer.getExtent());
		renderPassBeginInfo.clearValueCount = static_cast<uint32>(clearValues.count());

		if (clearValues.count() == 0) {
			renderPassBeginInfo.pClearValues = nullptr;
		}
		else {
			renderPassBeginInfo.pClearValues = &clearValues[0];
		}

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		commandCount += 1;
	}

	void VulkanCommandEncoder::bindComputePipeline(const ref<ComputePipeline>& computePipeline) {
		this->pipeline = std::dynamic_pointer_cast<VulkanPipeline>(computePipeline);

		vkCmdBindPipeline(commandBuffer, this->pipeline->bindPoint, this->pipeline->object);

		commandCount += 1;
	}

	void VulkanCommandEncoder::bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) {
		this->pipeline = std::dynamic_pointer_cast<VulkanPipeline>(renderPipeline);

		vkCmdBindPipeline(commandBuffer, this->pipeline->bindPoint, this->pipeline->object);

		commandCount += 1;
	}

	void VulkanCommandEncoder::bindDescriptorSets(const ref<PipelineLayout>& pipelineLayout, const sequence<ref<DescriptorSet>>& descriptorSets) {
		sequence<VkDescriptorSet> vkDescriptorSets(descriptorSets.count());

		for (uint32 descriptorSetIndex = 0; descriptorSetIndex < descriptorSets.count(); descriptorSetIndex += 1) {
			vkDescriptorSets[descriptorSetIndex] = std::static_pointer_cast<VulkanDescriptorSet>(descriptorSets[descriptorSetIndex])->object;
		}

		vkCmdBindDescriptorSets(
			commandBuffer,
			pipeline->bindPoint,
			std::static_pointer_cast<VulkanPipelineLayout>(pipelineLayout)->object,
			0,
			static_cast<uint32>(vkDescriptorSets.count()),
			&vkDescriptorSets[0],
			0,
			nullptr
		);

		commandCount += 1;
	}

	void VulkanCommandEncoder::bindVertexBuffer(uint32 bindingIndex, const ref<Buffer>& vertexBuffer, uint64 offset, uint32 stride) {
		vkCmdBindVertexBuffers(
			commandBuffer,
			bindingIndex,
			1,
			&std::static_pointer_cast<VulkanBuffer>(vertexBuffer)->object,
			&offset
		);

		commandCount += 1;
	}

	void VulkanCommandEncoder::copyBufferToBuffer(const ref<Buffer>& sourceBuffer, const ref<Buffer>& destinationBuffer, const sequence<BufferCopyRegion>& copyRegions) {
		vkCmdCopyBuffer(
			commandBuffer,
			std::static_pointer_cast<VulkanBuffer>(sourceBuffer)->object,
			std::static_pointer_cast<VulkanBuffer>(destinationBuffer)->object,
			static_cast<uint32>(copyRegions.count()),
			pointer_cast<const VkBufferCopy*>(&copyRegions[0])
		);

		commandCount += 1;
	}

	void VulkanCommandEncoder::copyBufferToImage(const ref<Buffer>& sourceBuffer, const ref<Image>& destinationImage, const sequence<BufferImageCopyRegion>& copyRegions) {
		const sequence<VkBufferImageCopy> bufferImageCopies = parseBufferImageCopyRegions(copyRegions);

		vkCmdCopyBufferToImage(
			commandBuffer,
			std::static_pointer_cast<VulkanBuffer>(sourceBuffer)->object,
			std::static_pointer_cast<VulkanImage>(destinationImage)->object,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			static_cast<uint32>(bufferImageCopies.count()),
			&bufferImageCopies[0]
		);

		commandCount += 1;
	}

	void VulkanCommandEncoder::copyImageToBuffer(const ref<Image>& sourceImage, const ref<Buffer>& destinationBuffer, const sequence<BufferImageCopyRegion>& copyRegions) {
		const sequence<VkBufferImageCopy> bufferImageCopies = parseBufferImageCopyRegions(copyRegions);

		vkCmdCopyImageToBuffer(
			commandBuffer,
			std::static_pointer_cast<VulkanImage>(sourceImage)->object,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			std::static_pointer_cast<VulkanBuffer>(destinationBuffer)->object,
			static_cast<uint32>(bufferImageCopies.count()),
			&bufferImageCopies[0]
		);

		commandCount += 1;
	}

	void VulkanCommandEncoder::dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) {
		ENGINE_ASSERT(groupCountX <= limits.maxComputeWorkgroupCount[0], "Group count in the X axis must not exceed [].", limits.maxComputeWorkgroupCount[0]);
		ENGINE_ASSERT(groupCountY <= limits.maxComputeWorkgroupCount[1], "Group count in the Y axis must not exceed [].", limits.maxComputeWorkgroupCount[1]);
		ENGINE_ASSERT(groupCountZ <= limits.maxComputeWorkgroupCount[2], "Group count in the Z axis must not exceed [].", limits.maxComputeWorkgroupCount[2]);

		vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);

		commandCount += 1;
	}

	void VulkanCommandEncoder::dispatchIndirect(const ref<Buffer>& buffer, uint64 offset) {
		vkCmdDispatchIndirect(commandBuffer, std::static_pointer_cast<VulkanBuffer>(buffer)->object, offset);

		commandCount += 1;
	}

	void VulkanCommandEncoder::nextSubpass() {
		vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

		commandCount += 1;
	}

	void VulkanCommandEncoder::setImageMemoryBarrier(const ref<Image>& image, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) {
		VulkanImage& vkImage = *std::static_pointer_cast<VulkanImage>(image);

		VkImageMemoryBarrier imageMemoryBarrier{};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.pNext = nullptr;
		imageMemoryBarrier.srcAccessMask = getVkAccessFlags(image->getLayout());
		imageMemoryBarrier.dstAccessMask = getVkAccessFlags(newLayout);
		imageMemoryBarrier.oldLayout = getVkImageLayout(image->getLayout());
		imageMemoryBarrier.newLayout = getVkImageLayout(newLayout);
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.image = vkImage.object;
		imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
		imageMemoryBarrier.subresourceRange.levelCount = image->getMipLevelCount();
		imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
		imageMemoryBarrier.subresourceRange.layerCount = image->getLayerCount();

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

		vkImage.setLayout(newLayout);

		commandCount += 1;
	}

	void VulkanCommandEncoder::setLineWidth(float32 lineWidth) {
		vkCmdSetLineWidth(commandBuffer, lineWidth);

		commandCount += 1;
	}

	void VulkanCommandEncoder::setScissor(sint32 xOffset, sint32 yOffset, Extent2D extent) {
		VkRect2D scissor{};
		scissor.offset.x = xOffset;
		scissor.offset.y = yOffset;
		scissor.extent = *pointer_cast<const VkExtent2D*>(&extent);

		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		commandCount += 1;
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

		commandCount += 1;
	}

	void VulkanCommandEncoder::draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) {
		vkCmdDraw(commandBuffer, vertexCount, instanceCount, vertexOffset, instanceOffset);

		commandCount += 1;
	}

	void VulkanCommandEncoder::drawIndirect(const ref<Buffer>& buffer, uint64 offset, uint32 drawCount, uint32 stride) {
		vkCmdDrawIndirect(commandBuffer, std::static_pointer_cast<VulkanBuffer>(buffer)->object, offset, drawCount, stride);

		commandCount += 1;
	}

	void VulkanCommandEncoder::drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) {
		vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, indexOffset, indexBias, instanceOffset);

		commandCount += 1;
	}

	void VulkanCommandEncoder::drawIndexedIndirect(const ref<Buffer>& buffer, uint64 offset, uint32 drawCount, uint32 stride) {
		vkCmdDrawIndexedIndirect(commandBuffer, std::static_pointer_cast<VulkanBuffer>(buffer)->object, offset, drawCount, stride);

		commandCount += 1;
	}

	void VulkanCommandEncoder::endRenderPass() {
		vkCmdEndRenderPass(commandBuffer);

		commandCount += 1;
	}

	ref<CommandBuffer> VulkanCommandEncoder::end() const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VkResult result = vkEndCommandBuffer(commandBuffer);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to end Vulkan command buffer ([]).", string_VkResult(result));

		VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		if (commandBufferUsageFlag & VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT) {
			pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}

		return createRef<VulkanCommandBuffer>(commandBuffer, pipelineStageFlags, commandCount);
	}

	void VulkanCommandEncoder::reset() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		commandCount = 0;
		pipeline = nullptr;

		const VkResult result = vkResetCommandBuffer(commandBuffer, 0);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to reset Vulkan command buffer ([]).", string_VkResult(result));
	}

	void VulkanCommandEncoder::bindIndexBuffer(const ref<Buffer>& indexBuffer, uint64 offset, uint8 indexTypeSize) {
		vkCmdBindIndexBuffer(
			commandBuffer,
			std::static_pointer_cast<VulkanBuffer>(indexBuffer)->object,
			offset,
			getVkIndexType(indexTypeSize)
		);

		commandCount += 1;
	}

	sequence<VkBufferImageCopy> VulkanCommandEncoder::parseBufferImageCopyRegions(const sequence<BufferImageCopyRegion>& copyRegions) {
		sequence<VkBufferImageCopy> bufferImageCopies(copyRegions.count());

		for (uint32 bufferImageCopyIndex = 0; bufferImageCopyIndex < bufferImageCopies.count(); bufferImageCopyIndex += 1) {
			const BufferImageCopyRegion& copyRegion = copyRegions[bufferImageCopyIndex];
			VkBufferImageCopy& bufferImageCopy = bufferImageCopies[bufferImageCopyIndex];

			bufferImageCopy.bufferOffset = copyRegion.bufferOffset;
			bufferImageCopy.bufferRowLength = 0;
			bufferImageCopy.bufferImageHeight = 0;
			bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferImageCopy.imageSubresource.mipLevel = copyRegion.imageMipLevel;
			bufferImageCopy.imageSubresource.baseArrayLayer = copyRegion.imageLayerOffset;
			bufferImageCopy.imageSubresource.layerCount = copyRegion.imageLayerCount;
			bufferImageCopy.imageOffset.x = copyRegion.imageOffsetX;
			bufferImageCopy.imageOffset.y = copyRegion.imageOffsetY;
			bufferImageCopy.imageOffset.z = copyRegion.imageOffsetZ;
			bufferImageCopy.imageExtent = *pointer_cast<const VkExtent3D*>(&copyRegion.imageExtent);
		}

		return bufferImageCopies;
	}
}