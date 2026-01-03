// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/DX12/Fence.hpp"
#include "Engine/Adapter/Graphics/DX12/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/DX12/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/DX12/RenderPipeline.hpp"
#include "Engine/Adapter/Graphics/DX12/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/DX12/Swapchain.hpp"

namespace ProjectA {
	DX12::GraphicsAdapter::GraphicsAdapter(const ProjectDescriptor& projectDescriptor, const Window& window) :
		ProjectA::GraphicsAdapter(window)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		logger = ConsoleLogger(std::cout, LogLevel::ALL, "DIRECTX 12");

		instance = createRef<DX12::Instance>();
		surface = createRef<DX12::Surface>(*instance, window);
		device = createRef<DX12::Device>(*instance);
	}

	DX12::GraphicsAdapter::~GraphicsAdapter() {
		ENGINE_ERROR_UNIMPLEMENTED();
	}

	ref<Buffer> DX12::GraphicsAdapter::createBuffer(const BufferDescriptor& descriptor) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<ComputePipeline> DX12::GraphicsAdapter::createComputePipeline(const ComputePipelineDescriptor& descriptor) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<DescriptorSetLayout> DX12::GraphicsAdapter::createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<Fence> DX12::GraphicsAdapter::createFence() const {
		return createRef<DX12::Fence>(*device);
	}

	ref<Framebuffer> DX12::GraphicsAdapter::createFramebuffer(const FramebufferDescriptor& descriptor) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<Image> DX12::GraphicsAdapter::createImage(const ImageDescriptor& descriptor) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<PipelineLayout> DX12::GraphicsAdapter::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const {
		return createRef<DX12::PipelineLayout>(descriptor, *device);
	}

	ref<RenderPass> DX12::GraphicsAdapter::createRenderPass(const RenderPassDescriptor& descriptor) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<RenderPipeline> DX12::GraphicsAdapter::createRenderPipeline(const RenderPipelineDescriptor& descriptor) const {
		return createRef<DX12::RenderPipeline>(descriptor, *device);
	}

	ref<Sampler> DX12::GraphicsAdapter::createSampler(const SamplerDescriptor& descriptor) const {
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<Semaphore> DX12::GraphicsAdapter::createSemaphore() const {
		return createRef<Semaphore>();
	}

	ref<ShaderModule> DX12::GraphicsAdapter::createShaderModule(const ref<ShaderModuleAsset>& asset, const ShaderModuleDescriptor& descriptor) const {
		ADAPTER_ASSERT(false, "Shader module assets are not supported within DX12 pipelines.");

		return nullptr;
	}

	ref<ShaderModule> DX12::GraphicsAdapter::createShaderModule(const sequence<uint8>& code, const ShaderModuleDescriptor& descriptor) const {
		return createRef<DX12::ShaderModule>(code, descriptor);
	}

	ref<Swapchain> DX12::GraphicsAdapter::createSwapchain(const SwapchainDescriptor& descriptor) const {
		return createRef<DX12::Swapchain>(descriptor, *instance, *surface, *device);
	}

	ref<Device> DX12::GraphicsAdapter::getDevice() const {
		return device;
	}

	ref<Surface> DX12::GraphicsAdapter::getSurface() const {
		return surface;
	}
}