#include "pch.hpp"

#include "Backend/Vulkan/VulkanBuffer.hpp"
#include "Backend/Vulkan/VulkanCommandEncoder.hpp"
#include "Backend/Vulkan/VulkanFence.hpp"
#include "Backend/Vulkan/VulkanRendererAPI.hpp"
#include "Backend/Vulkan/VulkanRenderPipeline.hpp"
#include "Backend/Vulkan/VulkanSemaphore.hpp"
#include "Backend/Vulkan/VulkanShaderModule.hpp"
#include "Backend/Vulkan/VulkanTexture.hpp"

namespace ProjectA {
	VulkanRendererAPI::VulkanRendererAPI(GLFWwindow* window) :
		instance(),
		physicalDevice(instance.object),
		surface(instance.object, physicalDevice, window),
		device(createRef<VulkanDevice>(physicalDevice, surface)),
		commandPool(physicalDevice, device->object)
	{
		// 1. Create instance.
		// 2. Select physical device.
		// 3. Create surface.
		// 4. Check if the physical device supports the surface.
		// 5. Get capabilities, extent, format and present mode of the surface with the physical device.
		// 6. Create device.
		// 7. Create command pool.

		// 1. Create swapchain.
		// 2. Create render pass.
		// 3. Create pipeline layout.
		// 4. Create graphics pipeline.
		// 5. Create buffer.
		// 6. Allocate memory for the buffer.
	}

	ref<Device> VulkanRendererAPI::getDevice() const {
		return device;
	}

	ref<Buffer> VulkanRendererAPI::createBuffer(const BufferDescriptor& descriptor) const {
		return createRef<VulkanBuffer>(physicalDevice.object, commandPool.object, *device, descriptor);
	}

	ref<CommandEncoder> VulkanRendererAPI::createCommandEncoder(const CommandBufferDescriptor& descriptor) const {
		return createRef<VulkanCommandEncoder>(*device, commandPool.object, descriptor);
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

	ref<Semaphore> VulkanRendererAPI::createSemaphore() const {
		return createRef<VulkanSemaphore>(device->object);
	}

	ref<ShaderModule> VulkanRendererAPI::createShaderModule(const ShaderStage stage, const std::string& code) const {
		static constexpr VkShaderStageFlagBits shaderStages[] = {
			VK_SHADER_STAGE_VERTEX_BIT,
			VK_SHADER_STAGE_FRAGMENT_BIT,
		};

		return createRef<VulkanShaderModule>(device->object, shaderStages[static_cast<uint32>(stage)], code);
	}

	ref<Swapchain> VulkanRendererAPI::createSwapchain(const SwapchainDescriptor& descriptor) const {
		return createRef<VulkanSwapchain>(physicalDevice, surface, device->object, descriptor);
	}

	ref<Texture> VulkanRendererAPI::createTexture(const TextureDescriptor& descriptor) const {
		return createRef<VulkanTexture>(*device, descriptor);
	}
}