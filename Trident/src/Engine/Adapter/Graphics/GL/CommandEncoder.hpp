// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"
#include "Engine/Adapter/Graphics/GL/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/GL/Device.hpp"
#include "Engine/Adapter/Graphics/GL/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/GL/GraphicsPipeline.hpp"
#include "Engine/Adapter/Graphics/GL/RenderPass.hpp"
#include "Engine/API/Graphics/CommandEncoder.hpp"

namespace Trident {
	class GL::CommandEncoder final : public Trident::CommandEncoder {
		public:
			explicit CommandEncoder(const Trident::Device* device, const Trident::CommandPool* commandPool);

			template<typename CommandType>
			void addCommand(ref<CommandType> command);

		public:
			ref<GL::Framebuffer> framebuffer = nullptr;
			ref<GL::RenderPass> renderPass = nullptr;
			uint32 subpassIndex = 0;

			#if defined(DEBUG_GL_ENABLE_COMMAND_QUEUE)
				sequence<ref<GL::Command>> commands;
				uint64 commandCount = 0;
			#endif

			ref<GL::ComputePipeline> computePipeline = nullptr;
			ref<GL::GraphicsPipeline> graphicsPipeline = nullptr;
			GLuint indexOffset = 0;
			GLenum indexType = 0;
			GLsizei indexTypeSize = 0;
			PhysicalDeviceLimits limits;
	};

	namespace GL {
		void begin(const Trident::CommandEncoder* commandEncoder);

		void beginRenderPass(Trident::CommandEncoder* commandEncoder, const BeginRenderPassDescriptor& descriptor);

		void bindComputePipeline(Trident::CommandEncoder* commandEncoder, ref<Trident::ComputePipeline> computePipeline);

		void bindDescriptorSets(Trident::CommandEncoder* commandEncoder, ref<Trident::PipelineLayout> pipelineLayout, uint32 descriptorSetIndexOffset, view<ref<Trident::DescriptorSet>> descriptorSets);

		void bindGraphicsPipeline(Trident::CommandEncoder* commandEncoder, ref<Trident::GraphicsPipeline> graphicsPipeline);

		void bindIndexBuffer(Trident::CommandEncoder* commandEncoder, ref<Trident::Buffer> indexBuffer, uint64 offset, IndexFormat format);

		void bindVertexBuffer(Trident::CommandEncoder* commandEncoder, uint32 bindingIndex, ref<Trident::Buffer> vertexBuffer, uint64 offset);

		void copyBufferToBuffer(Trident::CommandEncoder* commandEncoder, ref<Trident::Buffer> sourceBuffer, ref<Trident::Buffer> destinationBuffer, view<BufferCopyRegion> copyRegions);

		void copyBufferToImage(Trident::CommandEncoder* commandEncoder, ref<Trident::Buffer> sourceBuffer, ref<Trident::Image> destinationImage, view<BufferImageCopyRegion> copyRegions);

		void copyImageToBuffer(Trident::CommandEncoder* commandEncoder, ref<Trident::Image> sourceImage, ref<Trident::Buffer> destinationBuffer, view<BufferImageCopyRegion> copyRegions);

		ref<Trident::CommandEncoder> createCommandEncoder(const Trident::Device* device, const Trident::CommandPool* commandPool, const CommandBufferDescriptor& descriptor);

		void destroyCommandEncoder(const Trident::Device* device, const Trident::CommandPool* commandPool, const Trident::CommandEncoder* commandEncoder);

		void dispatch(Trident::CommandEncoder* commandEncoder, uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ);

		void dispatchIndirect(Trident::CommandEncoder* commandEncoder, ref<Trident::Buffer> buffer, uint64 bufferOffset);

		void draw(Trident::CommandEncoder* commandEncoder, uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset);

		void drawIndexed(Trident::CommandEncoder* commandEncoder, uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset);

		void drawIndexedIndirect(Trident::CommandEncoder* commandEncoder, ref<Trident::Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride);

		void drawIndirect(Trident::CommandEncoder* commandEncoder, ref<Trident::Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride);

		ref<Trident::CommandBuffer> end(const Trident::CommandEncoder* commandEncoder);

		void endRenderPass(Trident::CommandEncoder* commandEncoder);

		void insertMemoryBarriers(Trident::CommandEncoder* commandEncoder, view<BufferMemoryBarrier> bufferMemoryBarriers, view<ImageMemoryBarrier> imageMemoryBarriers);

		void nextSubpass(Trident::CommandEncoder* commandEncoder);

		void reset(Trident::CommandEncoder* commandEncoder);

		void setLineWidth(Trident::CommandEncoder* commandEncoder, float32 lineWidth);

		void setScissor(Trident::CommandEncoder* commandEncoder, sint32 xOffset, sint32 yOffset, Extent2D extent);

		void setViewport(Trident::CommandEncoder* commandEncoder, vec2<float32> origin, vec2<float32> size, float32 minDepth, float32 maxDepth);
	}
}