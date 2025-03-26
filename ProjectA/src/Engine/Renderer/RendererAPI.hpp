#pragma once

#include "Engine/API/CommandEncoder.hpp"
#include "Engine/API/Synchronization/Fence.hpp"
#include "Engine/API/Synchronization/Semaphore.hpp"
#include "Engine/Renderer/Buffer/Buffer.hpp"
#include "Engine/Renderer/Device.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/ShaderModule.hpp"
#include "Engine/Renderer/Texture.hpp"

namespace ProjectA {
	class RendererAPI {
		public:
			virtual ~RendererAPI() = default;

			virtual ref<Device> getDevice() const = 0;

			virtual ref<Buffer> createBuffer(const BufferDescriptor& descriptor) const = 0;

			virtual ref<CommandEncoder> createCommandEncoder(const CommandBufferDescriptor& descriptor) const = 0;

			virtual ref<Fence> createFence() const = 0;

			virtual ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const = 0;

			virtual ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const = 0;

			virtual ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const = 0;

			virtual ref<Sampler> createSampler(const SamplerDescriptor& descriptor) const = 0;

			virtual ref<Semaphore> createSemaphore() const = 0;

			virtual ref<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor, const chain<uint8>& code) const = 0;

			virtual ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) = 0;

			virtual ref<Texture> createTexture(const TextureDescriptor& descriptor) const = 0;
	};
}