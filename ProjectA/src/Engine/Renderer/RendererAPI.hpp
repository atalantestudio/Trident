#pragma once

#include "Engine/Renderer/Buffer/Buffer.hpp"
#include "Engine/Renderer/CommandBuffer.hpp"
#include "Engine/Renderer/Device.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"
#include "Engine/Renderer/ShaderStage.hpp"

namespace ProjectA {
	class RendererAPI {
		public:
			virtual ~RendererAPI() = default;

			virtual ref<Device> getDevice() const = 0;

			// (Vulkan) This cannot be const because allocation alters the device's memory.
			virtual ref<Buffer> createBuffer(const BufferDescriptor& descriptor) = 0;

			virtual ref<CommandBuffer> createCommandBuffer() const = 0;

			virtual ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const = 0;

			virtual ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const = 0;

			virtual ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const = 0;

			virtual ref<ShaderModule> createShaderModule(const ShaderStage stage, const std::string& code) const = 0;

			virtual ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const = 0;
	};
}