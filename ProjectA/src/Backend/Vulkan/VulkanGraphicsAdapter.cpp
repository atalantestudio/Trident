#include "pch.hpp"

#include "Backend/Vulkan/VulkanBuffer.hpp"
#include "Backend/Vulkan/VulkanCommandEncoder.hpp"
#include "Backend/Vulkan/VulkanComputePipeline.hpp"
#include "Backend/Vulkan/VulkanFence.hpp"
#include "Backend/Vulkan/VulkanFramebuffer.hpp"
#include "Backend/Vulkan/VulkanGraphicsAdapter.hpp"
#include "Backend/Vulkan/VulkanImage.hpp"
#include "Backend/Vulkan/VulkanRenderPipeline.hpp"
#include "Backend/Vulkan/VulkanSampler.hpp"
#include "Backend/Vulkan/VulkanSemaphore.hpp"
#include "Backend/Vulkan/VulkanShaderModule.hpp"

#include "Backend/Vulkan/MemoryAllocator.hpp"

namespace ProjectA {
	VulkanGraphicsAdapter::VulkanGraphicsAdapter(const Window& window) :
		instance(),
		surface(createRef<VulkanSurface>(instance.object, window)),
		physicalDevice(instance.object),
		device(createRef<VulkanDevice>(physicalDevice, *surface))
	{
		surface->checkPresentQueueFamilyIndex(physicalDevice.object, physicalDevice.graphicsQueueFamilyIndex);

		MemoryAllocator::initialize(physicalDevice);
	}

	VulkanGraphicsAdapter::~VulkanGraphicsAdapter() {
		MemoryAllocator::destroy();
	}

	ref<Buffer> VulkanGraphicsAdapter::createBuffer(const BufferDescriptor& descriptor) const {
		return createRef<VulkanBuffer>(device->object, descriptor);
	}

	ref<ComputePipeline> VulkanGraphicsAdapter::createComputePipeline(const ComputePipelineDescriptor& descriptor) const {
		return createRef<VulkanComputePipeline>(device->object, descriptor);
	}

	ref<Fence> VulkanGraphicsAdapter::createFence() const {
		return createRef<VulkanFence>(device->object);
	}

	ref<Framebuffer> VulkanGraphicsAdapter::createFramebuffer(const FramebufferDescriptor& descriptor) const {
		return createRef<VulkanFramebuffer>(device->object, descriptor);
	}

	ref<Image> VulkanGraphicsAdapter::createImage(const ImageDescriptor& descriptor) const {
		return createRef<VulkanImage>(device->object, descriptor);
	}

	ref<PipelineLayout> VulkanGraphicsAdapter::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const {
		return createRef<VulkanPipelineLayout>(device->object, descriptor);
	}

	ref<RenderPass> VulkanGraphicsAdapter::createRenderPass(const RenderPassDescriptor& descriptor) const {
		return createRef<VulkanRenderPass>(physicalDevice, *surface, *device, descriptor);
	}

	ref<RenderPipeline> VulkanGraphicsAdapter::createRenderPipeline(const RenderPipelineDescriptor& descriptor) const {
		return createRef<VulkanRenderPipeline>(device->object, descriptor);
	}

	ref<Sampler> VulkanGraphicsAdapter::createSampler(const SamplerDescriptor& descriptor) const {
		return createRef<VulkanSampler>(device->object, descriptor);
	}

	ref<Semaphore> VulkanGraphicsAdapter::createSemaphore() const {
		return createRef<VulkanSemaphore>(device->object);
	}

	ref<ShaderModule> VulkanGraphicsAdapter::createShaderModule(const sequence<uint8>& code, const ShaderModuleDescriptor& descriptor) const {
		return createRef<VulkanShaderModule>(device->object, code, descriptor);
	}

	ref<Swapchain> VulkanGraphicsAdapter::createSwapchain(const SwapchainDescriptor& descriptor) const {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		return createRef<VulkanSwapchain>(physicalDevice, *surface, *device, descriptor);
	}

	ref<Device> VulkanGraphicsAdapter::getDevice() const {
		return device;
	}

	ref<Surface> VulkanGraphicsAdapter::getSurface() const {
		return surface;
	}
}