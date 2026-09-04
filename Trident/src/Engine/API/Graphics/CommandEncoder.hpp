// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Buffer.hpp"
#include "Engine/API/Graphics/BufferCopyRegion.hpp"
#include "Engine/API/Graphics/BufferImageCopyRegion.hpp"
#include "Engine/API/Graphics/BufferMemoryBarrier.hpp"
#include "Engine/API/Graphics/CommandBuffer.hpp"
#include "Engine/API/Graphics/ComputePipeline.hpp"
#include "Engine/API/Graphics/Descriptor/BeginRenderPassDescriptor.hpp"
#include "Engine/API/Graphics/Descriptor/CommandBufferDescriptor.hpp"
#include "Engine/API/Graphics/DescriptorSet.hpp"
#include "Engine/API/Graphics/GraphicsPipeline.hpp"
#include "Engine/API/Graphics/Image.hpp"
#include "Engine/API/Graphics/ImageLayout.hpp"
#include "Engine/API/Graphics/ImageMemoryBarrier.hpp"

namespace Trident {
	class CommandEncoder {
		public:
			~CommandEncoder();

			void begin() const;

			void beginRenderPass(const BeginRenderPassDescriptor& descriptor);

			// TODO: Either merge bindComputePipeline and bindGraphicsPipeline,
			// or implement RenderPassCommandEncoder and ComputePassCommandEncoder.
			void bindComputePipeline(ref<ComputePipeline> computePipeline);

			/// Binds descriptor sets to the command buffer.
			/// 
			/// NOTE
			/// The pipeline layout could have been taken implicitly from the current pipeline as well,
			/// but in Vulkan this operation requires a valid layout. This method asks for the pipeline layout to make sure the user keeps it around at runtime.
			void bindDescriptorSets(ref<PipelineLayout> pipelineLayout, uint32 descriptorSetIndexOffset, view<ref<DescriptorSet>> descriptorSets);

			void bindGraphicsPipeline(ref<GraphicsPipeline> graphicsPipeline);

			/// Binds an index buffer.
			template<typename IndexType>
			inline void bindIndexBuffer(
				/// The index buffer to bind.
				ref<Buffer> indexBuffer,

				/// The offset in bytes in the buffer. It must be a multiple of IndexType.
				uint64 offset
			) {
				#if defined(DEBUG_ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
					{
						Validator validator;

						if (offset % sizeof(IndexType) > 0) {
							VALIDATION_ERROR("The offset must be a multiple of [].", sizeof(IndexType));
						}
					}
				#endif

				bindIndexBuffer(indexBuffer, offset, sizeof(IndexType));
			}

			void bindVertexBuffer(uint32 bindingIndex, ref<Buffer> vertexBuffer, uint64 offset);

			void copyBufferToBuffer(ref<Buffer> sourceBuffer, ref<Buffer> destinationBuffer, view<BufferCopyRegion> copyRegions);

			void copyBufferToImage(ref<Buffer> sourceBuffer, ref<Image> destinationImage, view<BufferImageCopyRegion> copyRegions);

			void copyImageToBuffer(ref<Image> sourceImage, ref<Buffer> destinationBuffer, view<BufferImageCopyRegion> copyRegions);

			// TODO(graphics/command-encoder/dispatch): Replace the parameters with Extend3D.
			void dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ);

			void dispatchIndirect(ref<Buffer> buffer, uint64 bufferOffset);

			/// Draws primitives.
			void draw(
				/// The number of vertices to draw per instance.
				uint32 vertexCount,

				/// The number of instances to draw.
				uint32 instanceCount,

				/// The index of the first vertex to draw.
				uint32 vertexOffset,

				/// The index of the first instance to draw.
				uint32 instanceOffset
			);

			// Draws primitives with indexed vertices.
			// - `indexCount` is the number of indices to draw per instance.
			// - `instanceCount` is the number of instances to draw.
			// - `indexOffset` is the offset of the first index, in index units.
			// - `indexBias` is a value added to the index before indexing into the vertex buffer.
			// - `instanceOffset` is the offset of the first instance, in instance units.
			// TODO(graphics/dx12/index-bias): The bias isn't applied to `SV_VertexID`, so the shader doesn't see the same value as `gl_VertexID`/`gl_VertexIndex`.
			void drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset);

			// Draws primitives with indexed vertices, using arguments from an indirect buffer.
			// - `buffer` is the buffer containing draw arguments.
			// - `bufferOffset` is the offset into `buffer` where arguments begin, in bytes.
			// - `drawCount` is the number of draw calls to execute. Cannot be 0.
			// - `stride` is the distance between successive sets of draw arguments, in bytes. Ignored if `drawCount` is 0 or 1.
			void drawIndexedIndirect(ref<Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride);

			// Draws primitives using arguments from an indirect buffer.
			// - `buffer` is the buffer containing draw arguments.
			// - `bufferOffset` is the offset into `buffer` where arguments begin, in bytes.
			// - `drawCount` is the number of draw calls to execute. Cannot be 0.
			// - `stride` is the distance between successive sets of draw arguments, in bytes. Ignored if `drawCount` is 0 or 1.
			void drawIndirect(ref<Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride);

			ref<CommandBuffer> end() const;

			void endRenderPass();

			void nextSubpass();

			void insertMemoryBarriers(view<BufferMemoryBarrier> bufferMemoryBarriers, view<ImageMemoryBarrier> imageMemoryBarriers);

			void reset();

			// TODO: Deprecate (not supported by DirectX12).
			void setLineWidth(float32 lineWidth);

			// Updates the scissor area.
			// NOTE: This should only be used if the render pipeline's dynamic state contains SCISSOR.
			void setScissor(sint32 xOffset, sint32 yOffset, Extent2D extent);

			// Updates the viewport area.
			// NOTE: This should only be used if the render pipeline's dynamic state contains VIEWPORT.
			void setViewport(vec2<float32> origin, vec2<float32> size, float32 minDepth, float32 maxDepth);

		protected:
			explicit CommandEncoder(const Device* device, const CommandPool* commandPool);

		private:
			void bindIndexBuffer(ref<Buffer> indexBuffer, uint64 offset, uint8 indexTypeSize);

		public:
			const Device* device;
			const CommandPool* commandPool;

		private:
			const Framebuffer* currentFramebuffer = nullptr;
	};

	template void CommandEncoder::bindIndexBuffer<uint16>(ref<Buffer> indexBuffer, uint64 offset);
	template void CommandEncoder::bindIndexBuffer<uint32>(ref<Buffer> indexBuffer, uint64 offset);
}