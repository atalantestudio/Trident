#pragma once

#include "Engine/API/Rendering/Buffer.hpp"
#include "Engine/API/Rendering/BufferCopyRegion.hpp"
#include "Engine/API/Rendering/BufferImageCopyRegion.hpp"
#include "Engine/API/Rendering/CommandBuffer.hpp"
#include "Engine/API/Rendering/ComputePipeline.hpp"
#include "Engine/API/Rendering/Descriptor/BeginRenderPassDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/CommandBufferDescriptor.hpp"
#include "Engine/API/Rendering/Image.hpp"
#include "Engine/API/Rendering/ImageLayout.hpp"
#include "Engine/API/Rendering/PipelineStage.hpp"
#include "Engine/API/Rendering/RenderPipeline.hpp"

namespace ProjectA {
	class CommandEncoder {
		public:
			virtual ~CommandEncoder() = default;

			virtual void begin() const = 0;

			virtual void beginRenderPass(const BeginRenderPassDescriptor& descriptor) = 0;

			// TODO(review): Either merge bindComputePipeline and bindRenderPipeline,
			// or implement RenderPassCommandEncoder and ComputePassCommandEncoder.
			virtual void bindComputePipeline(const ref<ComputePipeline>& computePipeline) = 0;

			virtual void bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) = 0;

			// Binds all descriptor sets linked to the current pipeline.
			// TODO(review): Change the API to take in arbitrary descriptor sets.
			// This would mean keeping the descriptor sets alive in the client-side renderer class.
			virtual void bindAllDescriptorSets() = 0;

			// Binds the provided index buffer.
			// The `offset` is in bytes and must be a multiple of IndexType.
			template<typename IndexType>
			void bindIndexBuffer(const ref<Buffer>& indexBuffer, uint64 offset) {
				bindIndexBuffer(indexBuffer, offset, sizeof(IndexType));
			}

			// `stride` is only used by OpenGL when calling glBindVertexBuffer.
			virtual void bindVertexBuffer(uint32 bindingIndex, const ref<Buffer>& vertexBuffer, uint64 offset, uint32 stride) = 0;

			virtual void copyBufferToBuffer(const ref<Buffer>& sourceBuffer, const ref<Buffer>& destinationBuffer, const sequence<BufferCopyRegion>& copyRegions) = 0;

			virtual void copyBufferToImage(const ref<Buffer>& sourceBuffer, const ref<Image>& destinationImage, const sequence<BufferImageCopyRegion>& copyRegions) = 0;

			virtual void copyImageToBuffer(const ref<Image>& sourceImage, const ref<Buffer>& destinationBuffer, const sequence<BufferImageCopyRegion>& copyRegions) = 0;

			virtual void dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) = 0;

			virtual void dispatchIndirect(const ref<Buffer>& buffer, uint64 offset) = 0;

			virtual void setImageMemoryBarrier(const ref<Image>& image, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) = 0;

			virtual void setLineWidth(float32 lineWidth) = 0;

			// Updates the scissor area.
			// NOTE: This should only be used if the render pipeline's dynamic state contains SCISSOR.
			virtual void setScissor(sint32 xOffset, sint32 yOffset, Extent2D extent) = 0;

			// Updates the viewport area.
			// NOTE: This should only be used if the render pipeline's dynamic state contains VIEWPORT.
			virtual void setViewport(const vec2<float32>& origin, const vec2<float32>& size, float32 minDepth, float32 maxDepth) = 0;

			// Draws primitives.
			// - `vertexCount` is the number of total vertices to draw.
			// - `instanceCount` is the number of instances to draw.
			// - `vertexOffset` is the offset in vertex units in vertex rate attributes at which to start drawing.
			// - `instanceOffset` is the offset of the first instance, in instance units.
			virtual void draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) = 0;

			// Draws primitives using arguments from an indirect buffer.
			// - `buffer` is the buffer containing draw arguments.
			// - `offset` is the offset into `buffer` where arguments begin, in bytes.
			// - `drawCount` is the number of draw calls to execute. Cannot be 0.
			// - `stride` is the distance between successive sets of draw arguments, in bytes. Ignored if `drawCount` is 0 or 1.
			virtual void drawIndirect(const ref<Buffer>& buffer, uint64 offset, uint32 drawCount, uint32 stride) = 0;

			// Draws primitives with indexed vertices.
			// - `indexCount` is the number of total indices to draw.
			// - `instanceCount` is the number of instances to draw.
			// - `indexOffset` is the offset of the first index, in index units.
			// - `indexBias` is a value added to the index before indexing into the vertex buffer.
			// - `instanceOffset` is the offset of the first instance, in instance units.
			virtual void drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) = 0;

			// Draws primitives with indexed vertices, using arguments from an indirect buffer.
			// - `buffer` is the buffer containing draw arguments.
			// - `offset` is the offset into `buffer` where arguments begin, in bytes.
			// - `drawCount` is the number of draw calls to execute. Cannot be 0.
			// - `stride` is the distance between successive sets of draw arguments, in bytes. Ignored if `drawCount` is 0 or 1.
			virtual void drawIndexedIndirect(const ref<Buffer>& buffer, uint64 offset, uint32 drawCount, uint32 stride) = 0;

			virtual void endRenderPass() = 0;

			virtual ref<CommandBuffer> end() const = 0;

			virtual void reset() = 0;

		protected:
			virtual void bindIndexBuffer(const ref<Buffer>& indexBuffer, uint64 offset, uint8 indexTypeSize) = 0;
	};

	template void CommandEncoder::bindIndexBuffer<uint16>(const ref<Buffer>& indexBuffer, uint64 offset);
	template void CommandEncoder::bindIndexBuffer<uint32>(const ref<Buffer>& indexBuffer, uint64 offset);
}