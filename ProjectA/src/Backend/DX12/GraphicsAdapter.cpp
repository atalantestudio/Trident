// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Backend/DX12/GraphicsAdapter.hpp"
#include "Backend/DX12/Swapchain.hpp"

namespace ProjectA {
	DX12::GraphicsAdapter::GraphicsAdapter(const ProjectDescriptor& projectDescriptor, const Window& window) :
		ProjectA::GraphicsAdapter(window)
	{
		logger = ConsoleLogger(std::cout, LogLevel::ALL, "DIRECTX 12");

		instance = createRef<Instance>();
		surface = createRef<Surface>(*instance, window);
		device = createRef<Device>(*instance);
	}

	DX12::GraphicsAdapter::~GraphicsAdapter() {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	ref<ProjectA::Buffer> DX12::GraphicsAdapter::createBuffer(const BufferDescriptor& descriptor) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<ProjectA::ComputePipeline> DX12::GraphicsAdapter::createComputePipeline(const ComputePipelineDescriptor& descriptor) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<ProjectA::DescriptorSetLayout> DX12::GraphicsAdapter::createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<ProjectA::Fence> DX12::GraphicsAdapter::createFence() const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<ProjectA::Framebuffer> DX12::GraphicsAdapter::createFramebuffer(const FramebufferDescriptor& descriptor) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<ProjectA::Image> DX12::GraphicsAdapter::createImage(const ImageDescriptor& descriptor) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<ProjectA::PipelineLayout> DX12::GraphicsAdapter::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<ProjectA::RenderPass> DX12::GraphicsAdapter::createRenderPass(const RenderPassDescriptor& descriptor) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<ProjectA::RenderPipeline> DX12::GraphicsAdapter::createRenderPipeline(const RenderPipelineDescriptor& descriptor) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<ProjectA::Sampler> DX12::GraphicsAdapter::createSampler(const SamplerDescriptor& descriptor) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<ProjectA::Semaphore> DX12::GraphicsAdapter::createSemaphore() const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<ProjectA::ShaderModule> DX12::GraphicsAdapter::createShaderModule(const ref<ShaderModuleAsset>& asset, const ShaderModuleDescriptor& descriptor) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<ProjectA::ShaderModule> DX12::GraphicsAdapter::createShaderModule(const sequence<uint8>& code, const ShaderModuleDescriptor& descriptor) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<ProjectA::Swapchain> DX12::GraphicsAdapter::createSwapchain(const SwapchainDescriptor& descriptor) const {
		return createRef<DX12::Swapchain>(descriptor, *instance, *surface, *device);
	}

	ref<ProjectA::Device> DX12::GraphicsAdapter::getDevice() const {
		return device;
	}

	ref<ProjectA::Surface> DX12::GraphicsAdapter::getSurface() const {
		return surface;
	}
}