#pragma once

#include "Engine/API/Rendering/ComputePipeline.hpp"
#include "Engine/API/Rendering/Descriptor/ComputePipelineDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/PipelineLayoutDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/RenderPipelineDescriptor.hpp"
#include "Engine/API/Rendering/Descriptor/SamplerDescriptor.hpp"
#include "Engine/API/Rendering/Device.hpp"
#include "Engine/API/Rendering/Framebuffer.hpp"
#include "Engine/API/Rendering/RenderPipeline.hpp"
#include "Engine/API/Rendering/ShaderModule.hpp"
#include "Engine/API/Rendering/Surface.hpp"
#include "Engine/Application/ApplicationManifest.hpp"

namespace ProjectA {
	class GraphicsAdapter {
		public:
			virtual ~GraphicsAdapter() {}

			virtual ref<Buffer> createBuffer(const BufferDescriptor& descriptor) const = 0;

			virtual ref<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor) const = 0;

			virtual ref<Fence> createFence() const = 0;

			virtual ref<Framebuffer> createFramebuffer(const FramebufferDescriptor& descriptor) const = 0;

			virtual ref<Image> createImage(const ImageDescriptor& descriptor) const = 0;

			virtual ref<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const = 0;

			virtual ref<RenderPass> createRenderPass(const RenderPassDescriptor& descriptor) const = 0;

			virtual ref<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) const = 0;

			virtual ref<Sampler> createSampler(const SamplerDescriptor& descriptor) const = 0;

			virtual ref<Semaphore> createSemaphore() const = 0;

			virtual ref<ShaderModule> createShaderModule(const sequence<uint8>& code, const ShaderModuleDescriptor& descriptor) const = 0;

			virtual ref<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) const = 0;

			virtual ref<Surface> getSurface() const = 0;

			virtual ref<Device> getDevice() const = 0;
	};
}