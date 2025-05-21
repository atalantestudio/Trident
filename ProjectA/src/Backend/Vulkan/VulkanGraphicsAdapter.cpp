#include "pch.hpp"

#include "Backend/Vulkan/VulkanBuffer.hpp"
#include "Backend/Vulkan/VulkanCommandEncoder.hpp"
#include "Backend/Vulkan/VulkanComputePipeline.hpp"
#include "Backend/Vulkan/VulkanFence.hpp"
#include "Backend/Vulkan/VulkanGraphicsAdapter.hpp"
#include "Backend/Vulkan/VulkanImage.hpp"
#include "Backend/Vulkan/VulkanRenderPipeline.hpp"
#include "Backend/Vulkan/VulkanSampler.hpp"
#include "Backend/Vulkan/VulkanSemaphore.hpp"
#include "Backend/Vulkan/VulkanShaderModule.hpp"

namespace ProjectA {
	VulkanGraphicsAdapter::VulkanGraphicsAdapter(const Window& window) :
		instance(),
		physicalDevice(instance.object),
		surface(instance.object, physicalDevice, window),
		device(createRef<VulkanDevice>(physicalDevice, surface))
	{}

	ref<Device> VulkanGraphicsAdapter::getDevice() const {
		return device;
	}

	ref<Buffer> VulkanGraphicsAdapter::createBuffer(const BufferDescriptor& descriptor) const {
		return createRef<VulkanBuffer>(physicalDevice.object, *device, descriptor);
	}

	ref<ComputePipeline> VulkanGraphicsAdapter::createComputePipeline(const ComputePipelineDescriptor& descriptor) const {
		return createRef<VulkanComputePipeline>(device->object, descriptor);
	}

	ref<Fence> VulkanGraphicsAdapter::createFence() const {
		return createRef<VulkanFence>(device->object);
	}

	ref<Image> VulkanGraphicsAdapter::createImage(const ImageDescriptor& descriptor) const {
		return createRef<VulkanImage>(*device, descriptor);
	}

	ref<PipelineLayout> VulkanGraphicsAdapter::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const {
		return createRef<VulkanPipelineLayout>(device->object, descriptor);
	}

	ref<RenderPass> VulkanGraphicsAdapter::createRenderPass(const RenderPassDescriptor& descriptor) const {
		return createRef<VulkanRenderPass>(physicalDevice, surface, *device, descriptor);
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

	ref<ShaderModule> VulkanGraphicsAdapter::createShaderModule(const chain<uint8>& code, const ShaderModuleDescriptor& descriptor) const {
		return createRef<VulkanShaderModule>(device->object, code, descriptor);
	}

	ref<Swapchain> VulkanGraphicsAdapter::createSwapchain(const SwapchainDescriptor& descriptor) const {
		return createRef<VulkanSwapchain>(physicalDevice, surface, *device, descriptor);
	}
}