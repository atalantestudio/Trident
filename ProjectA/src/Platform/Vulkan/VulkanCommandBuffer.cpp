#include "pch.hpp"

#include "Platform/Vulkan/VulkanBuffer.hpp"
#include "Platform/Vulkan/VulkanCommandBuffer.hpp"
#include "Platform/Vulkan/VulkanRenderPass.hpp"
#include "Platform/Vulkan/VulkanRenderPipeline.hpp"

namespace ProjectA {
	VulkanCommandBuffer::VulkanCommandBuffer(const VkDevice device, const VkCommandPool commandPool) :
		object(nullptr),
		device(device),
		commandPool(commandPool)
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = nullptr;
		commandBufferAllocateInfo.commandPool = commandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &object) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan command buffer.");
		}
	}

	VulkanCommandBuffer::~VulkanCommandBuffer() {
		vkFreeCommandBuffers(device, commandPool, 1, &object);
	}

	void VulkanCommandBuffer::begin() const {
		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = 0;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(object, &commandBufferBeginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin Vulkan command buffer.");
		}
	}

	void VulkanCommandBuffer::beginRenderPass(const BeginRenderPassDescriptor& descriptor) const {
		const VulkanSwapchain* swapchain = reinterpret_cast<const VulkanSwapchain*>(descriptor.swapchain.get());

		const uint32 imageIndex = 0;

		// TOOD
		// const VkResult result = vkAcquireNextImageKHR(device, swapchain->object, UINT64_MAX, availableImageSemaphore, VK_NULL_HANDLE, &imageIndex);

		VkClearValue clearValue{};

		_mm_store_ps(clearValue.color.float32, descriptor.clearColor.m);

		clearValue.depthStencil.depth = descriptor.depth;
		clearValue.depthStencil.stencil = descriptor.stencil;

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.renderPass = reinterpret_cast<const VulkanRenderPass*>(descriptor.renderPass.get())->object;
		renderPassBeginInfo.framebuffer = swapchain->framebuffers[imageIndex];
		renderPassBeginInfo.renderArea.offset = {0, 0};
		renderPassBeginInfo.renderArea.extent = swapchain->imageExtent;
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearValue;

		vkCmdBeginRenderPass(object, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanCommandBuffer::bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) const {
		vkCmdBindPipeline(object, VK_PIPELINE_BIND_POINT_GRAPHICS, reinterpret_cast<const VulkanRenderPipeline*>(renderPipeline.get())->object);
	}

	void VulkanCommandBuffer::bindVertexBuffer(const ref<Buffer>& vertexBuffer) const {
		const VkDeviceSize offset = 0;

		vkCmdBindVertexBuffers(object, 0, 1, &(reinterpret_cast<const VulkanBuffer*>(vertexBuffer.get())->object), &offset);
	}

	void VulkanCommandBuffer::setViewport(const vec2<float32>& origin, const vec2<float32>& size, const float32 minDepth, const float32 maxDepth) const {
		VkViewport viewport{};
		viewport.x = origin[0];
		viewport.x = origin[1];
		viewport.width = size[0];
		viewport.height = size[1];
		viewport.minDepth = minDepth;
		viewport.maxDepth = maxDepth;

		vkCmdSetViewport(object, 0, 1, &viewport);
	}

	void VulkanCommandBuffer::setScissor(const int32 xOffset, const int32 yOffset, const uint32 width, const uint32 height) const {
		VkRect2D scissor{};
		scissor.offset.x = xOffset;
		scissor.offset.y = yOffset;
		scissor.extent.width = width;
		scissor.extent.height = height;

		vkCmdSetScissor(object, 0, 1, &scissor);
	}

	void VulkanCommandBuffer::draw(const uint32 vertexCount, const uint32 instanceCount, const uint32 vertexOffset) const {
		vkCmdDraw(object, vertexCount, instanceCount, vertexOffset, 0);
	}

	void VulkanCommandBuffer::endRenderPass() const {
		vkCmdEndRenderPass(object);
	}

	void VulkanCommandBuffer::end() const {
		if (vkEndCommandBuffer(object) != VK_SUCCESS) {
			throw std::runtime_error("Failed to end Vulkan command buffer.");
		}
	}

	void VulkanCommandBuffer::reset() const {
		if (vkResetCommandBuffer(object, 0) != VK_SUCCESS) {
			throw std::runtime_error("Failed to reset Vulkan command buffer.");
		}
	}
}