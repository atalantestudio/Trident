// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Buffer.hpp"
#include "Engine/API/Rendering/BufferCopyRegion.hpp"
#include "Engine/API/Rendering/BufferImageCopyRegion.hpp"
#include "Engine/API/Rendering/CommandBuffer.hpp"
#include "Engine/API/Rendering/ComputePipeline.hpp"
#include "Engine/API/Rendering/Descriptor/BeginRenderPassDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/CommandBufferDescriptor.hpp"
#include "Engine/API/Rendering/DescriptorSet.hpp"
#include "Engine/API/Rendering/Image.hpp"
#include "Engine/API/Rendering/ImageLayout.hpp"
#include "Engine/API/Rendering/PipelineStage.hpp"
#include "Engine/API/Rendering/RenderPipeline.hpp"

namespace Trident {
	class CommandEncoder {
		public:
			virtual ~CommandEncoder() = default;

			virtual void begin() const = 0;

			virtual void beginRenderPass(const BeginRenderPassDescriptor& descriptor) = 0;

			// TODO: Either merge bindComputePipeline and bindRenderPipeline,
			// or implement RenderPassCommandEncoder and ComputePassCommandEncoder.
			virtual void bindComputePipeline(ref<ComputePipeline> computePipeline) = 0;

			virtual void bindRenderPipeline(ref<RenderPipeline> renderPipeline) = 0;

			/// Binds descriptor sets to the command buffer.
			/// 
			/// NOTE: The pipeline layout could have been taken from the current pipeline as well,
			/// but in Vulkan this operation requires a valid layout. This method asks for a layout to make sure the user keeps it around at runtime.
			virtual void bindDescriptorSets(ref<PipelineLayout> pipelineLayout, view<ref<DescriptorSet>> descriptorSets) = 0;

			// Binds the provided index buffer.
			// The `offset` is in bytes and must be a multiple of IndexType.
			template<typename IndexType>
			void bindIndexBuffer(ref<Buffer> indexBuffer, uint64 offset) {
				bindIndexBuffer(indexBuffer, offset, sizeof(IndexType));
			}

			// `stride` is only used by OpenGL when calling glBindVertexBuffer.
			virtual void bindVertexBuffer(uint32 bindingIndex, ref<Buffer> vertexBuffer, uint64 offset, uint32 stride) = 0;

			virtual void copyBufferToBuffer(ref<Buffer> sourceBuffer, ref<Buffer> destinationBuffer, view<BufferCopyRegion> copyRegions) = 0;

			virtual void copyBufferToImage(ref<Buffer> sourceBuffer, ref<Image> destinationImage, view<BufferImageCopyRegion> copyRegions) = 0;

			virtual void copyImageToBuffer(ref<Image> sourceImage, ref<Buffer> destinationBuffer, view<BufferImageCopyRegion> copyRegions) = 0;

			virtual void dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) = 0;

			virtual void dispatchIndirect(ref<Buffer> buffer, uint64 bufferOffset) = 0;

			virtual void nextSubpass() = 0;

			virtual void setImageMemoryBarrier(ref<Image> image, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) = 0;

			virtual void setLineWidth(float32 lineWidth) = 0;

			// Updates the scissor area.
			// NOTE: This should only be used if the render pipeline's dynamic state contains SCISSOR.
			virtual void setScissor(sint32 xOffset, sint32 yOffset, Extent2D extent) = 0;

			// Updates the viewport area.
			// NOTE: This should only be used if the render pipeline's dynamic state contains VIEWPORT.
			virtual void setViewport(vec2<float32> origin, vec2<float32> size, float32 minDepth, float32 maxDepth) = 0;

			// Draws primitives.
			// - `vertexCount` is the number of vertices to draw per instance.
			// - `instanceCount` is the number of instances to draw.
			// - `vertexOffset` is the offset in vertex units in vertex rate attributes at which to start drawing.
			// - `instanceOffset` is the offset of the first instance, in instance units.
			virtual void draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) = 0;

			// Draws primitives using arguments from an indirect buffer.
			// - `buffer` is the buffer containing draw arguments.
			// - `bufferOffset` is the offset into `buffer` where arguments begin, in bytes.
			// - `drawCount` is the number of draw calls to execute. Cannot be 0.
			// - `stride` is the distance between successive sets of draw arguments, in bytes. Ignored if `drawCount` is 0 or 1.
			virtual void drawIndirect(ref<Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) = 0;

			// Draws primitives with indexed vertices.
			// - `indexCount` is the number of indices to draw per instance.
			// - `instanceCount` is the number of instances to draw.
			// - `indexOffset` is the offset of the first index, in index units.
			// - `indexBias` is a value added to the index before indexing into the vertex buffer.
			// - `instanceOffset` is the offset of the first instance, in instance units.
			virtual void drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) = 0;

			// Draws primitives with indexed vertices, using arguments from an indirect buffer.
			// - `buffer` is the buffer containing draw arguments.
			// - `bufferOffset` is the offset into `buffer` where arguments begin, in bytes.
			// - `drawCount` is the number of draw calls to execute. Cannot be 0.
			// - `stride` is the distance between successive sets of draw arguments, in bytes. Ignored if `drawCount` is 0 or 1.
			virtual void drawIndexedIndirect(ref<Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) = 0;

			virtual void endRenderPass() = 0;

			virtual ref<CommandBuffer> end() const = 0;

			virtual void reset() = 0;

		protected:
			virtual void bindIndexBuffer(ref<Buffer> indexBuffer, uint64 offset, uint8 indexTypeSize) = 0;
	};

	template void CommandEncoder::bindIndexBuffer<uint16>(ref<Buffer> indexBuffer, uint64 offset);
	template void CommandEncoder::bindIndexBuffer<uint32>(ref<Buffer> indexBuffer, uint64 offset);
}