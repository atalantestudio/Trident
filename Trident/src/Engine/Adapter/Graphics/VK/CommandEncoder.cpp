// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/AccessFlags.hpp"
#include "Engine/Adapter/Graphics/VK/Buffer.hpp"
#include "Engine/Adapter/Graphics/VK/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/VK/CommandBufferUsage.hpp"
#include "Engine/Adapter/Graphics/VK/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/VK/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/VK/CommandPool.hpp"
#include "Engine/Adapter/Graphics/VK/DescriptorSet.hpp"
#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/VK/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/VK/GraphicsPipeline.hpp"
#include "Engine/Adapter/Graphics/VK/Image.hpp"
#include "Engine/Adapter/Graphics/VK/ImageLayout.hpp"
#include "Engine/Adapter/Graphics/VK/IndexType.hpp"
#include "Engine/Adapter/Graphics/VK/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/VK/PipelineStage.hpp"
#include "Engine/Adapter/Graphics/VK/RenderPass.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	static sequence<VkBufferImageCopy> parseBufferImageCopyRegions(view<BufferImageCopyRegion> copyRegions, VkImageAspectFlags aspectFlags) {
		sequence<VkBufferImageCopy> bufferImageCopies(copyRegions.count());
		uint32 bufferImageCopyIndex = 0;

		while (bufferImageCopyIndex < bufferImageCopies.count()) {
			const BufferImageCopyRegion& copyRegion = copyRegions[bufferImageCopyIndex];
			VkBufferImageCopy& bufferImageCopy = bufferImageCopies[bufferImageCopyIndex];

			bufferImageCopy.bufferOffset = copyRegion.bufferOffset;
			bufferImageCopy.bufferRowLength = 0;
			bufferImageCopy.bufferImageHeight = 0;
			bufferImageCopy.imageSubresource.aspectMask = aspectFlags;
			bufferImageCopy.imageSubresource.mipLevel = copyRegion.imageMipLevel;
			bufferImageCopy.imageSubresource.baseArrayLayer = copyRegion.imageLayerOffset;
			bufferImageCopy.imageSubresource.layerCount = copyRegion.imageLayerCount;
			bufferImageCopy.imageOffset.x = copyRegion.imageOffsetX;
			bufferImageCopy.imageOffset.y = copyRegion.imageOffsetY;
			bufferImageCopy.imageOffset.z = copyRegion.imageOffsetZ;
			bufferImageCopy.imageExtent = *pointer_cast<const VkExtent3D*>(&copyRegion.imageExtent);

			bufferImageCopyIndex += 1;
		}

		return bufferImageCopies;
	}

	VK::CommandEncoder::CommandEncoder(const Trident::Device* device, const Trident::CommandPool* commandPool) :
		Trident::CommandEncoder(device, commandPool)
	{}

	void VK::begin(const Trident::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkBeginCommandBuffer vkBeginCommandBuffer = loadDeviceProcedure<PFN_vkBeginCommandBuffer>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkBeginCommandBuffer");

		const VK::CommandEncoder* _commandEncoder = static_cast<const VK::CommandEncoder*>(commandEncoder);
		VkResult result;

		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.flags = _commandEncoder->commandBufferUsageFlag;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		result = vkBeginCommandBuffer(_commandEncoder->commandBuffer, &commandBufferBeginInfo);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to begin command buffer ([]).", string_VkResult(result));
	}

	void VK::beginRenderPass(Trident::CommandEncoder* commandEncoder, const BeginRenderPassDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass = loadDeviceProcedure<PFN_vkCmdBeginRenderPass>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdBeginRenderPass");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);
		const ref<VK::Framebuffer> framebuffer = std::static_pointer_cast<VK::Framebuffer>(descriptor.framebuffer);

		sequence<VkClearValue> clearValues(descriptor.clearValues.count());
		uint64 clearValueIndex = 0;

		while (clearValueIndex < clearValues.count()) {
			VkClearValue& vkClearValue = clearValues[clearValueIndex];
			const ClearValue& clearValue = descriptor.clearValues[clearValueIndex];

			clearValueIndex += 1;

			if (clearValue.depth != 0.0f || clearValue.stencil != 0) {
				vkClearValue.depthStencil.depth = clearValue.depth;
				vkClearValue.depthStencil.stencil = clearValue.stencil;

				continue;
			}

			vkClearValue.color.float32[0] = clearValue.color[0];
			vkClearValue.color.float32[1] = clearValue.color[1];
			vkClearValue.color.float32[2] = clearValue.color[2];
			vkClearValue.color.float32[3] = clearValue.color[3];
		}

		const Extent2D renderAreaExtent = framebuffer->getExtent();

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.renderPass = std::static_pointer_cast<VK::RenderPass>(framebuffer->getRenderPass())->renderPass;
		renderPassBeginInfo.framebuffer = framebuffer->framebuffer;
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent = *pointer_cast<const VkExtent2D*>(&renderAreaExtent);
		renderPassBeginInfo.clearValueCount = static_cast<uint32>(clearValues.count());

		if (clearValues.count() == 0) {
			renderPassBeginInfo.pClearValues = nullptr;
		}
		else {
			renderPassBeginInfo.pClearValues = &clearValues[0];
		}

		vkCmdBeginRenderPass(_commandEncoder->commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		_commandEncoder->commandCount += 1;
	}

	void VK::bindComputePipeline(Trident::CommandEncoder* commandEncoder, ref<Trident::ComputePipeline> computePipeline) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdBindPipeline vkCmdBindPipeline = loadDeviceProcedure<PFN_vkCmdBindPipeline>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdBindPipeline");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);
		const ref<VK::ComputePipeline> _computePipeline = std::static_pointer_cast<VK::ComputePipeline>(computePipeline);

		vkCmdBindPipeline(_commandEncoder->commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, _computePipeline->pipeline);

		_commandEncoder->pipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
		_commandEncoder->commandCount += 1;
	}

	void VK::bindDescriptorSets(Trident::CommandEncoder* commandEncoder, ref<Trident::PipelineLayout> pipelineLayout, uint32 descriptorSetIndexOffset, view<ref<Trident::DescriptorSet>> descriptorSets) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets = loadDeviceProcedure<PFN_vkCmdBindDescriptorSets>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdBindDescriptorSets");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);
		sequence<VkDescriptorSet> vkDescriptorSets(descriptorSets.count());
		uint32 descriptorSetIndex = 0;

		while (descriptorSetIndex < descriptorSets.count()) {
			vkDescriptorSets[descriptorSetIndex] = std::static_pointer_cast<VK::DescriptorSet>(descriptorSets[descriptorSetIndex])->descriptorSet;

			descriptorSetIndex += 1;
		}

		vkCmdBindDescriptorSets(
			_commandEncoder->commandBuffer,
			_commandEncoder->pipelineBindPoint,
			std::static_pointer_cast<VK::PipelineLayout>(pipelineLayout)->pipelineLayout,
			descriptorSetIndexOffset,
			static_cast<uint32>(vkDescriptorSets.count()),
			&vkDescriptorSets[0],
			0,
			nullptr
		);

		_commandEncoder->commandCount += 1;
	}

	void VK::bindGraphicsPipeline(Trident::CommandEncoder* commandEncoder, ref<Trident::GraphicsPipeline> graphicsPipeline) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdBindPipeline vkCmdBindPipeline = loadDeviceProcedure<PFN_vkCmdBindPipeline>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdBindPipeline");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);
		const ref<VK::GraphicsPipeline> _graphicsPipeline = std::static_pointer_cast<VK::GraphicsPipeline>(graphicsPipeline);

		vkCmdBindPipeline(_commandEncoder->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline->pipeline);

		_commandEncoder->pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		_commandEncoder->commandCount += 1;
	}

	void VK::bindIndexBuffer(Trident::CommandEncoder* commandEncoder, ref<Trident::BaseBuffer> indexBuffer, uint64 offset, uint8 indexTypeSize) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer = loadDeviceProcedure<PFN_vkCmdBindIndexBuffer>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdBindIndexBuffer");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkCmdBindIndexBuffer(
			_commandEncoder->commandBuffer,
			std::static_pointer_cast<VK::BaseBuffer>(indexBuffer)->buffer,
			offset,
			getIndexType(indexTypeSize)
		);

		_commandEncoder->commandCount += 1;
	}

	void VK::bindVertexBuffer(Trident::CommandEncoder* commandEncoder, uint32 bindingIndex, ref<Trident::BaseBuffer> vertexBuffer, uint64 offset, uint32 stride) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers = loadDeviceProcedure<PFN_vkCmdBindVertexBuffers>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdBindVertexBuffers");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkCmdBindVertexBuffers(
			_commandEncoder->commandBuffer,
			bindingIndex,
			1,
			&std::static_pointer_cast<VK::BaseBuffer>(vertexBuffer)->buffer,
			&offset
		);

		_commandEncoder->commandCount += 1;
	}

	void VK::copyBufferToBuffer(Trident::CommandEncoder* commandEncoder, ref<Trident::BaseBuffer> sourceBuffer, ref<Trident::BaseBuffer> destinationBuffer, view<BufferCopyRegion> copyRegions) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdCopyBuffer vkCmdCopyBuffer = loadDeviceProcedure<PFN_vkCmdCopyBuffer>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdCopyBuffer");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkCmdCopyBuffer(
			_commandEncoder->commandBuffer,
			std::static_pointer_cast<VK::BaseBuffer>(sourceBuffer)->buffer,
			std::static_pointer_cast<VK::BaseBuffer>(destinationBuffer)->buffer,
			static_cast<uint32>(copyRegions.count()),
			pointer_cast<const VkBufferCopy*>(&copyRegions[0])
		);

		_commandEncoder->commandCount += 1;
	}

	void VK::copyBufferToImage(Trident::CommandEncoder* commandEncoder, ref<Trident::BaseBuffer> sourceBuffer, ref<Trident::Image> destinationImage, view<BufferImageCopyRegion> copyRegions) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage = loadDeviceProcedure<PFN_vkCmdCopyBufferToImage>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdCopyBufferToImage");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);
		const ref<VK::Image> vkImage = std::static_pointer_cast<VK::Image>(destinationImage);
		const sequence<VkBufferImageCopy> bufferImageCopies = parseBufferImageCopyRegions(copyRegions, vkImage->aspectFlags);

		vkCmdCopyBufferToImage(
			_commandEncoder->commandBuffer,
			std::static_pointer_cast<VK::BaseBuffer>(sourceBuffer)->buffer,
			vkImage->image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			static_cast<uint32>(bufferImageCopies.count()),
			&bufferImageCopies[0]
		);

		_commandEncoder->commandCount += 1;
	}

	void VK::copyImageToBuffer(Trident::CommandEncoder* commandEncoder, ref<Trident::Image> sourceImage, ref<Trident::BaseBuffer> destinationBuffer, view<BufferImageCopyRegion> copyRegions) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer = loadDeviceProcedure<PFN_vkCmdCopyImageToBuffer>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdCopyImageToBuffer");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);
		const ref<VK::Image> vkImage = std::static_pointer_cast<VK::Image>(sourceImage);
		const sequence<VkBufferImageCopy> bufferImageCopies = parseBufferImageCopyRegions(copyRegions, vkImage->aspectFlags);

		vkCmdCopyImageToBuffer(
			_commandEncoder->commandBuffer,
			vkImage->image,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			std::static_pointer_cast<VK::BaseBuffer>(destinationBuffer)->buffer,
			static_cast<uint32>(bufferImageCopies.count()),
			&bufferImageCopies[0]
		);

		_commandEncoder->commandCount += 1;
	}

	ref<Trident::CommandEncoder> VK::createCommandEncoder(const Trident::Device* device, const Trident::CommandPool* commandPool, const CommandBufferDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::CommandPool* _commandPool = static_cast<const VK::CommandPool*>(commandPool);
		VkResult result;

		ref<VK::CommandEncoder> commandEncoder = createRef<VK::CommandEncoder>(device, commandPool);
		commandEncoder->commandBufferUsageFlag = getCommandBufferUsageFlag(descriptor.usage);

		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = nullptr;
		commandBufferAllocateInfo.commandPool = _commandPool->commandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;

		result = _device->vkAllocateCommandBuffers(_device->device, &commandBufferAllocateInfo, &commandEncoder->commandBuffer);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create command buffer ([]).", string_VkResult(result));

		#if defined(DEBUG_OBJECT_NAME)
			if (descriptor.name.count() > 0) {
				VkDebugUtilsObjectNameInfoEXT debugUtilsObjectNameInfo{};
				debugUtilsObjectNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
				debugUtilsObjectNameInfo.pNext = nullptr;
				debugUtilsObjectNameInfo.objectType = VK_OBJECT_TYPE_COMMAND_BUFFER;
				debugUtilsObjectNameInfo.objectHandle = (uint64) commandEncoder->commandBuffer;
				debugUtilsObjectNameInfo.pObjectName = &descriptor.name[0];

				result = _device->vkSetDebugUtilsObjectNameEXT(_device->device, &debugUtilsObjectNameInfo);

				ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to set object name ([]).", string_VkResult(result));
			}
		#endif

		return commandEncoder;
	}

	void VK::destroyCommandEncoder(const Trident::Device* device, const Trident::CommandPool* commandPool, const Trident::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::CommandPool* _commandPool = static_cast<const VK::CommandPool*>(commandPool);
		const VK::CommandEncoder* _commandEncoder = static_cast<const VK::CommandEncoder*>(commandEncoder);

		_device->vkFreeCommandBuffers(_device->device, _commandPool->commandPool, 1, &_commandEncoder->commandBuffer);
	}

	void VK::dispatch(Trident::CommandEncoder* commandEncoder, uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdDispatch vkCmdDispatch = loadDeviceProcedure<PFN_vkCmdDispatch>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdDispatch");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		// TODO: Move in Trident::CommandEncoder::dispatch.
		// ADAPTER_ASSERT(groupCountX <= limits.maxComputeWorkgroupCount[0], "Group count in the X axis must not exceed [].", limits.maxComputeWorkgroupCount[0]);
		// ADAPTER_ASSERT(groupCountY <= limits.maxComputeWorkgroupCount[1], "Group count in the Y axis must not exceed [].", limits.maxComputeWorkgroupCount[1]);
		// ADAPTER_ASSERT(groupCountZ <= limits.maxComputeWorkgroupCount[2], "Group count in the Z axis must not exceed [].", limits.maxComputeWorkgroupCount[2]);

		vkCmdDispatch(_commandEncoder->commandBuffer, groupCountX, groupCountY, groupCountZ);

		_commandEncoder->commandCount += 1;
	}

	void VK::dispatchIndirect(Trident::CommandEncoder* commandEncoder, ref<Trident::BaseBuffer> buffer, uint64 bufferOffset) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdDispatchIndirect vkCmdDispatchIndirect = loadDeviceProcedure<PFN_vkCmdDispatchIndirect>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdDispatchIndirect");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkCmdDispatchIndirect(_commandEncoder->commandBuffer, std::static_pointer_cast<VK::BaseBuffer>(buffer)->buffer, bufferOffset);

		_commandEncoder->commandCount += 1;
	}

	void VK::draw(Trident::CommandEncoder* commandEncoder, uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdDraw vkCmdDraw = loadDeviceProcedure<PFN_vkCmdDraw>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdDraw");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkCmdDraw(_commandEncoder->commandBuffer, vertexCount, instanceCount, vertexOffset, instanceOffset);

		_commandEncoder->commandCount += 1;
	}

	void VK::drawIndexed(Trident::CommandEncoder* commandEncoder, uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdDrawIndexed vkCmdDrawIndexed = loadDeviceProcedure<PFN_vkCmdDrawIndexed>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdDrawIndexed");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkCmdDrawIndexed(_commandEncoder->commandBuffer, indexCount, instanceCount, indexOffset, indexBias, instanceOffset);

		_commandEncoder->commandCount += 1;
	}

	void VK::drawIndexedIndirect(Trident::CommandEncoder* commandEncoder, ref<Trident::BaseBuffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect = loadDeviceProcedure<PFN_vkCmdDrawIndexedIndirect>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdDrawIndexedIndirect");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkCmdDrawIndexedIndirect(_commandEncoder->commandBuffer, std::static_pointer_cast<VK::BaseBuffer>(buffer)->buffer, bufferOffset, drawCount, stride);

		_commandEncoder->commandCount += 1;
	}

	void VK::drawIndirect(Trident::CommandEncoder* commandEncoder, ref<Trident::BaseBuffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdDrawIndirect vkCmdDrawIndirect = loadDeviceProcedure<PFN_vkCmdDrawIndirect>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdDrawIndirect");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkCmdDrawIndirect(_commandEncoder->commandBuffer, std::static_pointer_cast<VK::BaseBuffer>(buffer)->buffer, bufferOffset, drawCount, stride);

		_commandEncoder->commandCount += 1;
	}

	ref<Trident::CommandBuffer> VK::end(const Trident::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkEndCommandBuffer vkEndCommandBuffer = loadDeviceProcedure<PFN_vkEndCommandBuffer>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkEndCommandBuffer");

		const VK::CommandEncoder* _commandEncoder = static_cast<const VK::CommandEncoder*>(commandEncoder);
		VkResult result;

		result = vkEndCommandBuffer(_commandEncoder->commandBuffer);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to end command buffer ([]).", string_VkResult(result));

		VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

		if (_commandEncoder->commandBufferUsageFlag & VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT) {
			pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}

		return createRef<VK::CommandBuffer>(_commandEncoder->commandBuffer, pipelineStageFlags, _commandEncoder->commandCount);
	}

	void VK::endRenderPass(Trident::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdEndRenderPass vkCmdEndRenderPass = loadDeviceProcedure<PFN_vkCmdEndRenderPass>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdEndRenderPass");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkCmdEndRenderPass(_commandEncoder->commandBuffer);

		_commandEncoder->commandCount += 1;
	}

	void VK::insertMemoryBarriers(Trident::CommandEncoder* commandEncoder, view<BufferMemoryBarrier> bufferMemoryBarriers, view<ImageMemoryBarrier> imageMemoryBarriers) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier = loadDeviceProcedure<PFN_vkCmdPipelineBarrier>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdPipelineBarrier");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);
		sequence<VkBufferMemoryBarrier> vkBufferMemoryBarriers(bufferMemoryBarriers.count());
		index bufferMemoryBarrierIndex = 0;

		while (bufferMemoryBarrierIndex < bufferMemoryBarriers.count()) {
			const BufferMemoryBarrier& bufferMemoryBarrier = bufferMemoryBarriers[bufferMemoryBarrierIndex];

			VkBufferMemoryBarrier& vkBufferMemoryBarrier = vkBufferMemoryBarriers[bufferMemoryBarrierIndex];
			vkBufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			vkBufferMemoryBarrier.pNext = nullptr;
			vkBufferMemoryBarrier.srcAccessMask = getAccessFlags(bufferMemoryBarrier.buffer->access);
			vkBufferMemoryBarrier.dstAccessMask = getAccessFlags(bufferMemoryBarrier.newAccess);
			vkBufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			vkBufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			vkBufferMemoryBarrier.buffer = std::static_pointer_cast<VK::BaseBuffer>(bufferMemoryBarrier.buffer)->buffer;
			vkBufferMemoryBarrier.offset = bufferMemoryBarrier.offset;
			vkBufferMemoryBarrier.size = bufferMemoryBarrier.size;

			bufferMemoryBarrier.buffer->access = bufferMemoryBarrier.newAccess;

			bufferMemoryBarrierIndex += 1;
		}

		sequence<VkImageMemoryBarrier> vkImageMemoryBarriers(imageMemoryBarriers.count());
		index imageMemoryBarrierIndex = 0;

		while (imageMemoryBarrierIndex < imageMemoryBarriers.count()) {
			const ImageMemoryBarrier& imageMemoryBarrier = imageMemoryBarriers[imageMemoryBarrierIndex];
			const ref<VK::Image> image = std::static_pointer_cast<VK::Image>(imageMemoryBarrier.image);

			VkImageMemoryBarrier& vkImageMemoryBarrier = vkImageMemoryBarriers[imageMemoryBarrierIndex];
			vkImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			vkImageMemoryBarrier.pNext = nullptr;
			vkImageMemoryBarrier.srcAccessMask = getAccessFlags(imageMemoryBarrier.image->access);
			vkImageMemoryBarrier.dstAccessMask = getAccessFlags(imageMemoryBarrier.newAccess);
			vkImageMemoryBarrier.oldLayout = getImageLayout(imageMemoryBarrier.image->getLayout());
			vkImageMemoryBarrier.newLayout = getImageLayout(imageMemoryBarrier.newLayout);
			vkImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			vkImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			vkImageMemoryBarrier.image = image->image;
			vkImageMemoryBarrier.subresourceRange.aspectMask = image->aspectFlags;
			vkImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
			vkImageMemoryBarrier.subresourceRange.levelCount = imageMemoryBarrier.image->getMipLevelCount();
			vkImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
			vkImageMemoryBarrier.subresourceRange.layerCount = imageMemoryBarrier.image->getLayerCount();

			imageMemoryBarrier.image->layout = imageMemoryBarrier.newLayout;
			imageMemoryBarrier.image->access = imageMemoryBarrier.newAccess;

			imageMemoryBarrierIndex += 1;
		}

		vkCmdPipelineBarrier(
			_commandEncoder->commandBuffer,
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			0,
			0,
			nullptr,
			static_cast<uint32>(vkBufferMemoryBarriers.count()),
			&vkBufferMemoryBarriers[0],
			static_cast<uint32>(vkImageMemoryBarriers.count()),
			&vkImageMemoryBarriers[0]
		);

		_commandEncoder->commandCount += 1;
	}

	void VK::nextSubpass(Trident::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdNextSubpass vkCmdNextSubpass = loadDeviceProcedure<PFN_vkCmdNextSubpass>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdNextSubpass");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkCmdNextSubpass(_commandEncoder->commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

		_commandEncoder->commandCount += 1;
	}

	void VK::reset(Trident::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkResetCommandBuffer vkResetCommandBuffer = loadDeviceProcedure<PFN_vkResetCommandBuffer>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkResetCommandBuffer");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);
		VkResult result;

		result = vkResetCommandBuffer(_commandEncoder->commandBuffer, 0);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to reset command buffer ([]).", string_VkResult(result));

		_commandEncoder->commandCount = 0;
		_commandEncoder->pipelineBindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;
	}

	void VK::setLineWidth(Trident::CommandEncoder* commandEncoder, float32 lineWidth) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdSetLineWidth vkCmdSetLineWidth = loadDeviceProcedure<PFN_vkCmdSetLineWidth>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdSetLineWidth");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkCmdSetLineWidth(_commandEncoder->commandBuffer, lineWidth);

		_commandEncoder->commandCount += 1;
	}

	void VK::setScissor(Trident::CommandEncoder* commandEncoder, sint32 xOffset, sint32 yOffset, Extent2D extent) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdSetScissor vkCmdSetScissor = loadDeviceProcedure<PFN_vkCmdSetScissor>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdSetScissor");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		VkRect2D scissor{};
		scissor.offset.x = xOffset;
		scissor.offset.y = yOffset;
		scissor.extent = *pointer_cast<const VkExtent2D*>(&extent);

		vkCmdSetScissor(_commandEncoder->commandBuffer, 0, 1, &scissor);

		_commandEncoder->commandCount += 1;
	}

	void VK::setViewport(Trident::CommandEncoder* commandEncoder, vec2<float32> origin, vec2<float32> size, float32 minDepth, float32 maxDepth) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkCmdSetViewport vkCmdSetViewport = loadDeviceProcedure<PFN_vkCmdSetViewport>(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdSetViewport");

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		VkViewport viewport{};
		viewport.x = origin[0];
		viewport.y = origin[1];
		viewport.width = size[0];
		viewport.height = size[1];
		viewport.minDepth = minDepth;
		viewport.maxDepth = maxDepth;

		vkCmdSetViewport(_commandEncoder->commandBuffer, 0, 1, &viewport);

		_commandEncoder->commandCount += 1;
	}
}