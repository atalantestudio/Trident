#include "pch.hpp"

#include "Backend/Vulkan/VulkanBuffer.hpp"
#include "Backend/Vulkan/VulkanCommandEncoder.hpp"
#include "Backend/Vulkan/VulkanComputePipeline.hpp"
#include "Backend/Vulkan/VulkanFence.hpp"
#include "Backend/Vulkan/VulkanRendererAPI.hpp"
#include "Backend/Vulkan/VulkanRenderPipeline.hpp"
#include "Backend/Vulkan/VulkanSampler.hpp"
#include "Backend/Vulkan/VulkanSemaphore.hpp"
#include "Backend/Vulkan/VulkanShaderModule.hpp"
#include "Backend/Vulkan/VulkanTexture.hpp"

namespace ProjectA {
	VulkanRendererAPI::VulkanRendererAPI(GLFWwindow* window) :
		instance(),
		physicalDevice(instance.object),
		surface(instance.object, physicalDevice, window),
		device(createRef<VulkanDevice>(physicalDevice, surface))
	{}

	ref<Device> VulkanRendererAPI::getDevice() const {
		return device;
	}

	ref<Buffer> VulkanRendererAPI::createBuffer(const BufferDescriptor& descriptor) const {
		return createRef<VulkanBuffer>(physicalDevice.object, *device, descriptor);
	}

	ref<ComputePipeline> VulkanRendererAPI::createComputePipeline(const ComputePipelineDescriptor& descriptor) const {
		return createRef<VulkanComputePipeline>(device->object, descriptor);
	}

	ref<Fence> VulkanRendererAPI::createFence() const {
		return createRef<VulkanFence>(device->object);
	}

	ref<PipelineLayout> VulkanRendererAPI::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const {
		return createRef<VulkanPipelineLayout>(device->object, descriptor);
	}

	ref<RenderPass> VulkanRendererAPI::createRenderPass(const RenderPassDescriptor& descriptor) const {
		return createRef<VulkanRenderPass>(surface, device->object, descriptor);
	}

	ref<RenderPipeline> VulkanRendererAPI::createRenderPipeline(const RenderPipelineDescriptor& descriptor) const {
		return createRef<VulkanRenderPipeline>(device->object, descriptor);
	}

	ref<Sampler> VulkanRendererAPI::createSampler(const SamplerDescriptor& descriptor) const {
		return createRef<VulkanSampler>(device->object, descriptor);
	}

	ref<Semaphore> VulkanRendererAPI::createSemaphore() const {
		return createRef<VulkanSemaphore>(device->object);
	}

	ref<ShaderModule> VulkanRendererAPI::createShaderModule(const ShaderModuleDescriptor& descriptor, const chain<uint8>& code) const {
		return createRef<VulkanShaderModule>(device->object, descriptor, code);
	}

	ref<Swapchain> VulkanRendererAPI::createSwapchain(const SwapchainDescriptor& descriptor) {
		return createRef<VulkanSwapchain>(physicalDevice, surface, device->object, descriptor);
	}

	ref<Texture> VulkanRendererAPI::createTexture(const TextureDescriptor& descriptor) const {
		return createRef<VulkanTexture>(*device, descriptor);
	}
}