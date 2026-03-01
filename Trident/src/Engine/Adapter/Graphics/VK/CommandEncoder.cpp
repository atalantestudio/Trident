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
#include "Engine/Adapter/Graphics/VK/Pipeline.hpp"
#include "Engine/Adapter/Graphics/VK/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/VK/PipelineStage.hpp"
#include "Engine/Adapter/Graphics/VK/RenderPass.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	static sequence<VkBufferImageCopy> parseBufferImageCopyRegions(view<BufferImageCopyRegion> copyRegions) {
		sequence<VkBufferImageCopy> bufferImageCopies(copyRegions.count());
		uint32 bufferImageCopyIndex = 0;

		while (bufferImageCopyIndex < bufferImageCopies.count()) {
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

			bufferImageCopyIndex += 1;
		}

		return bufferImageCopies;
	}

	VK::CommandEncoder::CommandEncoder(const Trident::Device* device, const Trident::CommandPool* commandPool) :
		Trident::CommandEncoder(device, commandPool)
	{}

	void VK::begin(const Trident::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static const PFN_vkBeginCommandBuffer vkBeginCommandBuffer = static_cast<PFN_vkBeginCommandBuffer>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkBeginCommandBuffer"));

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

		static const PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass = static_cast<PFN_vkCmdBeginRenderPass>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdBeginRenderPass"));

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

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.renderPass = std::static_pointer_cast<VK::RenderPass>(framebuffer->getRenderPass())->renderPass;
		renderPassBeginInfo.framebuffer = framebuffer->framebuffer;
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent = *pointer_cast<const VkExtent2D*>(&framebuffer->getExtent());
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

		static const PFN_vkCmdBindPipeline vkCmdBindPipeline = static_cast<PFN_vkCmdBindPipeline>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdBindPipeline"));

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);
		const ref<VK::ComputePipeline> _computePipeline = std::static_pointer_cast<VK::ComputePipeline>(computePipeline);

		vkCmdBindPipeline(_commandEncoder->commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, _computePipeline->pipeline);

		_commandEncoder->pipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
		_commandEncoder->commandCount += 1;
	}

	void VK::bindDescriptorSets(Trident::CommandEncoder* commandEncoder, ref<Trident::PipelineLayout> pipelineLayout, uint32 descriptorSetIndexOffset, view<ref<Trident::DescriptorSet>> descriptorSets) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static const PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets = static_cast<PFN_vkCmdBindDescriptorSets>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdBindDescriptorSets"));

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

		static const PFN_vkCmdBindPipeline vkCmdBindPipeline = static_cast<PFN_vkCmdBindPipeline>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdBindPipeline"));

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);
		const ref<VK::GraphicsPipeline> _graphicsPipeline = std::static_pointer_cast<VK::GraphicsPipeline>(graphicsPipeline);

		vkCmdBindPipeline(_commandEncoder->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline->pipeline);

		_commandEncoder->pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		_commandEncoder->commandCount += 1;
	}

	void VK::bindIndexBuffer(Trident::CommandEncoder* commandEncoder, ref<Trident::BaseBuffer> indexBuffer, uint64 offset, uint8 indexTypeSize) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static const PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer = static_cast<PFN_vkCmdBindIndexBuffer>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdBindIndexBuffer"));

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

		static const PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers = static_cast<PFN_vkCmdBindVertexBuffers>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdBindVertexBuffers"));

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

		static const PFN_vkCmdCopyBuffer vkCmdCopyBuffer = static_cast<PFN_vkCmdCopyBuffer>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdCopyBuffer"));

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

		static const PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage = static_cast<PFN_vkCmdCopyBufferToImage>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdCopyBufferToImage"));

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);
		const sequence<VkBufferImageCopy> bufferImageCopies = parseBufferImageCopyRegions(copyRegions);

		vkCmdCopyBufferToImage(
			_commandEncoder->commandBuffer,
			std::static_pointer_cast<VK::BaseBuffer>(sourceBuffer)->buffer,
			std::static_pointer_cast<VK::Image>(destinationImage)->image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			static_cast<uint32>(bufferImageCopies.count()),
			&bufferImageCopies[0]
		);

		_commandEncoder->commandCount += 1;
	}

	void VK::copyImageToBuffer(Trident::CommandEncoder* commandEncoder, ref<Trident::Image> sourceImage, ref<Trident::BaseBuffer> destinationBuffer, view<BufferImageCopyRegion> copyRegions) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static const PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer = static_cast<PFN_vkCmdCopyImageToBuffer>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdCopyImageToBuffer"));

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);
		const sequence<VkBufferImageCopy> bufferImageCopies = parseBufferImageCopyRegions(copyRegions);

		vkCmdCopyImageToBuffer(
			_commandEncoder->commandBuffer,
			std::static_pointer_cast<VK::Image>(sourceImage)->image,
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

		#if defined(BUILD_MODE_DEBUG)
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

		static const PFN_vkCmdDispatch vkCmdDispatch = static_cast<PFN_vkCmdDispatch>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdDispatch"));

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

		static const PFN_vkCmdDispatchIndirect vkCmdDispatchIndirect = static_cast<PFN_vkCmdDispatchIndirect>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdDispatchIndirect"));

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkCmdDispatchIndirect(_commandEncoder->commandBuffer, std::static_pointer_cast<VK::BaseBuffer>(buffer)->buffer, bufferOffset);

		_commandEncoder->commandCount += 1;
	}

	void VK::draw(Trident::CommandEncoder* commandEncoder, uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static const PFN_vkCmdDraw vkCmdDraw = static_cast<PFN_vkCmdDraw>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdDraw"));

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkCmdDraw(_commandEncoder->commandBuffer, vertexCount, instanceCount, vertexOffset, instanceOffset);

		_commandEncoder->commandCount += 1;
	}

	void VK::drawIndexed(Trident::CommandEncoder* commandEncoder, uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static const PFN_vkCmdDrawIndexed vkCmdDrawIndexed = static_cast<PFN_vkCmdDrawIndexed>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdDrawIndexed"));

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkCmdDrawIndexed(_commandEncoder->commandBuffer, indexCount, instanceCount, indexOffset, indexBias, instanceOffset);

		_commandEncoder->commandCount += 1;
	}

	void VK::drawIndexedIndirect(Trident::CommandEncoder* commandEncoder, ref<Trident::BaseBuffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static const PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect = static_cast<PFN_vkCmdDrawIndexedIndirect>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdDrawIndexedIndirect"));

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkCmdDrawIndexedIndirect(_commandEncoder->commandBuffer, std::static_pointer_cast<VK::BaseBuffer>(buffer)->buffer, bufferOffset, drawCount, stride);

		_commandEncoder->commandCount += 1;
	}

	void VK::drawIndirect(Trident::CommandEncoder* commandEncoder, ref<Trident::BaseBuffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static const PFN_vkCmdDrawIndirect vkCmdDrawIndirect = static_cast<PFN_vkCmdDrawIndirect>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdDrawIndirect"));

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkCmdDrawIndirect(_commandEncoder->commandBuffer, std::static_pointer_cast<VK::BaseBuffer>(buffer)->buffer, bufferOffset, drawCount, stride);

		_commandEncoder->commandCount += 1;
	}

	ref<Trident::CommandBuffer> VK::end(const Trident::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static const PFN_vkEndCommandBuffer vkEndCommandBuffer = static_cast<PFN_vkEndCommandBuffer>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkEndCommandBuffer"));

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

		static const PFN_vkCmdEndRenderPass vkCmdEndRenderPass = static_cast<PFN_vkCmdEndRenderPass>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdEndRenderPass"));

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkCmdEndRenderPass(_commandEncoder->commandBuffer);

		_commandEncoder->commandCount += 1;
	}

	void VK::insertBufferMemoryBarriers(Trident::CommandEncoder* commandEncoder, view<BufferMemoryBarrier> barriers) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static const PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier = static_cast<PFN_vkCmdPipelineBarrier>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdPipelineBarrier"));

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);
		sequence<VkBufferMemoryBarrier> bufferMemoryBarriers(barriers.count());
		index barrierIndex = 0;

		while (barrierIndex < barriers.count()) {
			const BufferMemoryBarrier& barrier = barriers[barrierIndex];

			VkBufferMemoryBarrier& bufferMemoryBarrier = bufferMemoryBarriers[barrierIndex];
			bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			bufferMemoryBarrier.pNext = nullptr;
			bufferMemoryBarrier.srcAccessMask = getAccessFlags(barrier.sourceAccess);
			bufferMemoryBarrier.dstAccessMask = getAccessFlags(barrier.destinationAccess);
			bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			bufferMemoryBarrier.buffer = std::static_pointer_cast<VK::BaseBuffer>(barrier.buffer)->buffer;
			bufferMemoryBarrier.offset = barrier.offset;
			bufferMemoryBarrier.size = barrier.size;

			barrierIndex += 1;
		}

		vkCmdPipelineBarrier(
			_commandEncoder->commandBuffer,
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			0,
			0,
			nullptr,
			static_cast<uint32>(bufferMemoryBarriers.count()),
			&bufferMemoryBarriers[0],
			0,
			nullptr
		);

		_commandEncoder->commandCount += 1;
	}

	void VK::nextSubpass(Trident::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static const PFN_vkCmdNextSubpass vkCmdNextSubpass = static_cast<PFN_vkCmdNextSubpass>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdNextSubpass"));

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkCmdNextSubpass(_commandEncoder->commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

		_commandEncoder->commandCount += 1;
	}

	void VK::reset(Trident::CommandEncoder* commandEncoder) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static const PFN_vkResetCommandBuffer vkResetCommandBuffer = static_cast<PFN_vkResetCommandBuffer>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkResetCommandBuffer"));

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);
		VkResult result;

		result = vkResetCommandBuffer(_commandEncoder->commandBuffer, 0);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to reset command buffer ([]).", string_VkResult(result));

		_commandEncoder->commandCount = 0;
		_commandEncoder->pipelineBindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;
	}

	void VK::setImageMemoryBarrier(Trident::CommandEncoder* commandEncoder, ref<Trident::Image> image, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static const PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier = static_cast<PFN_vkCmdPipelineBarrier>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdPipelineBarrier"));

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);
		const ref<VK::Image> vkImage = std::static_pointer_cast<VK::Image>(image);

		VkImageMemoryBarrier imageMemoryBarrier{};
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.pNext = nullptr;
		imageMemoryBarrier.srcAccessMask = getAccessFlags(image->getLayout());
		imageMemoryBarrier.dstAccessMask = getAccessFlags(newLayout);
		imageMemoryBarrier.oldLayout = getImageLayout(image->getLayout());
		imageMemoryBarrier.newLayout = getImageLayout(newLayout);
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.image = vkImage->image;
		imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
		imageMemoryBarrier.subresourceRange.levelCount = image->getMipLevelCount();
		imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
		imageMemoryBarrier.subresourceRange.layerCount = image->getLayerCount();

		vkCmdPipelineBarrier(
			_commandEncoder->commandBuffer,
			getPipelineStageFlagBits(sourceStage),
			getPipelineStageFlagBits(destinationStage),
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&imageMemoryBarrier
		);

		image->layout = newLayout;

		_commandEncoder->commandCount += 1;
	}

	void VK::setLineWidth(Trident::CommandEncoder* commandEncoder, float32 lineWidth) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static const PFN_vkCmdSetLineWidth vkCmdSetLineWidth = static_cast<PFN_vkCmdSetLineWidth>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdSetLineWidth"));

		VK::CommandEncoder* _commandEncoder = static_cast<VK::CommandEncoder*>(commandEncoder);

		vkCmdSetLineWidth(_commandEncoder->commandBuffer, lineWidth);

		_commandEncoder->commandCount += 1;
	}

	void VK::setScissor(Trident::CommandEncoder* commandEncoder, sint32 xOffset, sint32 yOffset, Extent2D extent) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static const PFN_vkCmdSetScissor vkCmdSetScissor = static_cast<PFN_vkCmdSetScissor>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdSetScissor"));

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

		static const PFN_vkCmdSetViewport vkCmdSetViewport = static_cast<PFN_vkCmdSetViewport>(loadDeviceProcedure(static_cast<const VK::Device*>(commandEncoder->device)->device, "vkCmdSetViewport"));

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