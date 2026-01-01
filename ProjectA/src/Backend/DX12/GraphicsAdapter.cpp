// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Backend/DX12/GraphicsAdapter.hpp"

namespace ProjectA::DX12 {
	GraphicsAdapter::GraphicsAdapter(const ProjectDescriptor& projectDescriptor, const Window& window) :
		ProjectA::GraphicsAdapter(window)
	{
		logger = ConsoleLogger(std::cout, LogLevel::ALL, "DIRECTX12");

		instance = createRef<Instance>();

		/* #ifdef DEBUG_ENABLE_MESSENGER
			instance->createDebugUtilsMessenger();
		#endif */

		// surface = createRef<Surface>(instance->object, window);
		// physicalDevice = createRef<PhysicalDevice>(instance->selectPhysicalDevice());
		device = createRef<Device>(instance);

		// surface->setPhysicalDevice(physicalDevice->object);
		// surface->checkPresentQueueFamilyIndex(physicalDevice->graphicsQueueFamilyIndex);

		// MemoryAllocator::initialize(*physicalDevice);
	}

	GraphicsAdapter::~GraphicsAdapter() {}

	ref<Buffer> GraphicsAdapter::createBuffer(const BufferDescriptor& descriptor) const {
		return nullptr;
	}

	ref<ComputePipeline> GraphicsAdapter::createComputePipeline(const ComputePipelineDescriptor& descriptor) const {
		return nullptr;
	}

	ref<DescriptorSetLayout> GraphicsAdapter::createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings) const {
		return nullptr;
	}

	ref<Fence> GraphicsAdapter::createFence() const {
		return nullptr;
	}

	ref<Framebuffer> GraphicsAdapter::createFramebuffer(const FramebufferDescriptor& descriptor) const {
		return nullptr;
	}

	ref<Image> GraphicsAdapter::createImage(const ImageDescriptor& descriptor) const {
		return nullptr;
	}

	ref<PipelineLayout> GraphicsAdapter::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const {
		return nullptr;
	}

	ref<RenderPass> GraphicsAdapter::createRenderPass(const RenderPassDescriptor& descriptor) const {
		return nullptr;
	}

	ref<RenderPipeline> GraphicsAdapter::createRenderPipeline(const RenderPipelineDescriptor& descriptor) const {
		return nullptr;
	}

	ref<Sampler> GraphicsAdapter::createSampler(const SamplerDescriptor& descriptor) const {
		return nullptr;
	}

	ref<Semaphore> GraphicsAdapter::createSemaphore() const {
		return nullptr;
	}

	ref<ShaderModule> GraphicsAdapter::createShaderModule(const ref<ShaderModuleAsset>& asset, const ShaderModuleDescriptor& descriptor) const {
		return nullptr;
	}
	ref<ShaderModule> GraphicsAdapter::createShaderModule(const sequence<uint8>& code, const ShaderModuleDescriptor& descriptor) const {
		return nullptr;
	}

	ref<Swapchain> GraphicsAdapter::createSwapchain(const SwapchainDescriptor& descriptor) const {
		return nullptr;
	}

	ref<ProjectA::Device> GraphicsAdapter::getDevice() const {
		return nullptr;
	}

	ref<Surface> GraphicsAdapter::getSurface() const {
		return nullptr;
	}
}