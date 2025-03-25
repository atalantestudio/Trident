#pragma once

#include "Engine/Renderer/Buffer/Buffer.hpp"
#include "Engine/Renderer/CommandBuffer.hpp"
#include "Engine/Renderer/CommandBufferUsage.hpp"
#include "Engine/Renderer/IndexType.hpp"
#include "Engine/Renderer/RenderPass.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/TextureLayout.hpp"

namespace ProjectA {
	struct CommandBufferDescriptor {
		CommandBufferUsage usage;
	};

	struct BeginRenderPassDescriptor {
		ref<RenderPass> renderPass;
		ref<Swapchain> swapchain;
		vec4<float32> clearColor;
		uint32 nextImageIndex;
	};

	class CommandEncoder {
		public:
			virtual ~CommandEncoder() = default;

			virtual void begin() const = 0;

			virtual void beginRenderPass(const BeginRenderPassDescriptor& descriptor) = 0;

			virtual void bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) = 0;

			virtual void bindDescriptorSets(uint32 descriptorSetIndex) const = 0;

			virtual void bindIndexBuffer(const ref<Buffer>& indexBuffer, uint64 offset, IndexType indexType) = 0;

			virtual void bindVertexBuffer(uint32 bindingIndex, const ref<Buffer>& vertexBuffer, uint32 stride) = 0;

			virtual void copyBufferToBuffer(const ref<Buffer>& sourceBuffer, const ref<Buffer>& destinationBuffer, uint64 sourceOffset, uint64 destinationOffset, uint64 size) = 0;

			virtual void copyBufferToTexture(const ref<Buffer>& sourceBuffer, const ref<Texture>& destinationTexture, uint32 width, uint32 height, uint32 depth) = 0;

			virtual void setTextureMemoryBarrier(const ref<Texture>& texture, TextureLayout oldLayout, TextureLayout newLayout) const = 0;

			virtual void setViewport(const vec2<float32>& origin, const vec2<float32>& size, float32 minDepth, float32 maxDepth) = 0;

			virtual void setScissor(sint32 xOffset, sint32 yOffset, uint32 width, uint32 height) = 0;

			virtual void draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset) = 0;

			virtual void drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset) = 0;

			virtual void endRenderPass() const = 0;

			virtual ref<CommandBuffer> end() const = 0;

			virtual void reset() = 0;
	};
}