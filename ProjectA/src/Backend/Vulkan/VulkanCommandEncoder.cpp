#include "pch.hpp"

#include "Backend/Vulkan/IndexType.hpp"
#include "Backend/Vulkan/VulkanBuffer.hpp"
#include "Backend/Vulkan/VulkanCommandBuffer.hpp"
#include "Backend/Vulkan/VulkanCommandEncoder.hpp"
#include "Backend/Vulkan/VulkanRenderPass.hpp"
#include "Backend/Vulkan/VulkanRenderPipeline.hpp"

namespace ProjectA {
	VulkanCommandEncoder::VulkanCommandEncoder(const VulkanDevice& device, const VkCommandPool commandPool) :
		device(device),
		commandPool(commandPool)
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[VulkanCommandEncoder]");

		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = nullptr;
		commandBufferAllocateInfo.commandPool = commandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(device.object, &commandBufferAllocateInfo, &commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan command buffer.");
		}
	}

	VulkanCommandEncoder::~VulkanCommandEncoder() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~VulkanCommandEncoder]");

		vkFreeCommandBuffers(device.object, commandPool, 1, &commandBuffer);
	}

	void VulkanCommandEncoder::begin() const {
		PROFILE_SCOPE(FUNC_SIG);

		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = 0;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin Vulkan command buffer.");
		}
	}

	void VulkanCommandEncoder::beginRenderPass(const BeginRenderPassDescriptor& descriptor) {
		PROFILE_SCOPE(FUNC_SIG);

		const VulkanSwapchain* swapchain = static_cast<const VulkanSwapchain*>(descriptor.swapchain.get());

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

	void VulkanCommandEncoder::bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) {
		PROFILE_SCOPE(FUNC_SIG);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<const VulkanRenderPipeline*>(renderPipeline.get())->object);

		pipelineLayout = static_cast<const VulkanRenderPipeline*>(renderPipeline.get())->pipelineLayout;
	}

	void VulkanCommandEncoder::bindDescriptorSets(const uint32 descriptorSetIndex) const {
		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout->object,

			// TODO: Allow custom descriptor set range to be bound.
			0,
			static_cast<uint32>(pipelineLayout->descriptorSets.size()),

			pipelineLayout->descriptorSets.data(),
			0,
			nullptr
		);
	}

	void VulkanCommandEncoder::bindIndexBuffer(const ref<Buffer>& indexBuffer, const uint64 offset, const IndexType indexType) {
		PROFILE_SCOPE(FUNC_SIG);

		vkCmdBindIndexBuffer(commandBuffer, static_cast<const VulkanBuffer*>(indexBuffer.get())->object, offset, getVkIndexType(indexType));
	}

	void VulkanCommandEncoder::bindVertexBuffer(const uint32 bindingIndex, const ref<Buffer>& vertexBuffer, const uint32 stride) {
		PROFILE_SCOPE(FUNC_SIG);

		const VkDeviceSize offset = 0;

		vkCmdBindVertexBuffers(commandBuffer, bindingIndex, 1, &static_cast<const VulkanBuffer*>(vertexBuffer.get())->object, &offset);
	}

	void VulkanCommandEncoder::setViewport(const vec2<float32>& origin, const vec2<float32>& size, const float32 minDepth, const float32 maxDepth) {
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

	void VulkanCommandEncoder::setScissor(const int32 xOffset, const int32 yOffset, const uint32 width, const uint32 height) {
		PROFILE_SCOPE(FUNC_SIG);

		VkRect2D scissor{};
		scissor.offset.x = xOffset;
		scissor.offset.y = yOffset;
		scissor.extent.width = width;
		scissor.extent.height = height;

		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void VulkanCommandEncoder::draw(const uint32 vertexCount, const uint32 instanceCount, const uint32 vertexOffset) {
		PROFILE_SCOPE(FUNC_SIG);

		vkCmdDraw(commandBuffer, vertexCount, instanceCount, vertexOffset, 0);
	}

	void VulkanCommandEncoder::drawIndexed(const uint32 indexCount, const uint32 instanceCount, const uint32 indexOffset) {
		PROFILE_SCOPE(FUNC_SIG);

		vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, indexOffset, 0, 0);
	}

	void VulkanCommandEncoder::endRenderPass() const {
		PROFILE_SCOPE(FUNC_SIG);

		vkCmdEndRenderPass(commandBuffer);
	}

	ref<CommandBuffer> VulkanCommandEncoder::end() const {
		PROFILE_SCOPE(FUNC_SIG);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to end Vulkan command buffer.");
		}

		return createRef<VulkanCommandBuffer>(commandBuffer);
	}

	void VulkanCommandEncoder::reset() {
		PROFILE_SCOPE(FUNC_SIG);

		pipelineLayout = nullptr;

		if (vkResetCommandBuffer(commandBuffer, 0) != VK_SUCCESS) {
			throw std::runtime_error("Failed to reset Vulkan command buffer.");
		}
	}
}