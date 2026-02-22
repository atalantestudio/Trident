// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Buffer.hpp"
#include "Engine/Adapter/Graphics/VK/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/VK/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/VK/DescriptorSetLayout.hpp"
#include "Engine/Adapter/Graphics/VK/Fence.hpp"
#include "Engine/Adapter/Graphics/VK/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/VK/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/VK/GraphicsPipeline.hpp"
#include "Engine/Adapter/Graphics/VK/Image.hpp"
#include "Engine/Adapter/Graphics/VK/MemoryAllocator.hpp"
#include "Engine/Adapter/Graphics/VK/PipelineLayout.hpp"
#include "Engine/Adapter/Graphics/VK/RenderPass.hpp"
#include "Engine/Adapter/Graphics/VK/Sampler.hpp"
#include "Engine/Adapter/Graphics/VK/Semaphore.hpp"
#include "Engine/Adapter/Graphics/VK/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/VK/Swapchain.hpp"

namespace Trident {
	VK::GraphicsAdapter::GraphicsAdapter(const ProjectDescriptor& projectDescriptor, const Window& window, const ApplicationProperties& properties) :
		Trident::GraphicsAdapter(window),
		vulkan1Library(Platform::openLibrary("vulkan-1.dll"))
	{
		logger = ConsoleLogger(std::cout, LogLevel::ALL, "VULKAN");

		loadBaseProcedures();

		instance = createRef<VK::Instance>(projectDescriptor, properties);

		loadInstanceProcedures();

		#if defined(DEBUG_ENABLE_MESSENGER)
			instance->createDebugUtilsMessenger();
		#endif

		surface = createRef<VK::Surface>(window, *instance);
		physicalDevice = createRef<VK::PhysicalDevice>(instance->selectPhysicalDevice());
		device = createRef<VK::Device>(*physicalDevice);

		surface->setPhysicalDevice(*physicalDevice);
		surface->checkPresentQueueFamilyIndex(physicalDevice->graphicsQueueFamilyIndex);

		MemoryAllocator::initialize(*physicalDevice);
	}

	/* VK::GraphicsAdapter::~GraphicsAdapter() {
		MemoryAllocator::destroy();
	} */

	ref<Buffer> VK::GraphicsAdapter::createBuffer(const BufferDescriptor& descriptor) const {
		return createRef<VK::Buffer>(descriptor, *device);
	}

	ref<ComputePipeline> VK::GraphicsAdapter::createComputePipeline(const ComputePipelineDescriptor& descriptor) const {
		return createRef<VK::ComputePipeline>(descriptor, *device);
	}

	ref<DescriptorSetLayout> VK::GraphicsAdapter::createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings) const {
		return createRef<VK::DescriptorSetLayout>(bindings, *device);
	}

	ref<Fence> VK::GraphicsAdapter::createFence(const FenceDescriptor& descriptor) const {
		return createRef<VK::Fence>(descriptor, *device);
	}

	ref<Framebuffer> VK::GraphicsAdapter::createFramebuffer(const FramebufferDescriptor& descriptor) const {
		return createRef<VK::Framebuffer>(descriptor, *device);
	}

	ref<Image> VK::GraphicsAdapter::createImage(const ImageDescriptor& descriptor) const {
		return createRef<VK::Image>(descriptor, *device);
	}

	ref<PipelineLayout> VK::GraphicsAdapter::createPipelineLayout(const PipelineLayoutDescriptor& descriptor) const {
		return createRef<VK::PipelineLayout>(descriptor, *device);
	}

	ref<RenderPass> VK::GraphicsAdapter::createRenderPass(const RenderPassDescriptor& descriptor) const {
		return createRef<VK::RenderPass>(descriptor, *surface, *device);
	}

	ref<GraphicsPipeline> VK::GraphicsAdapter::createGraphicsPipeline(const GraphicsPipelineDescriptor& descriptor) const {
		return createRef<VK::GraphicsPipeline>(descriptor, *device);
	}

	ref<Sampler> VK::GraphicsAdapter::createSampler(const SamplerDescriptor& descriptor) const {
		return createRef<VK::Sampler>(descriptor, *device);
	}

	ref<Semaphore> VK::GraphicsAdapter::createSemaphore() const {
		return createRef<VK::Semaphore>(*device);
	}

	ref<ShaderModule> VK::GraphicsAdapter::createShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) const {
		// TODO: Add support for SPIR-V shader module assets.
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<ShaderModule> VK::GraphicsAdapter::createShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code) const {
		return createRef<VK::ShaderModule>(descriptor, code, *device);
	}

	ref<Swapchain> VK::GraphicsAdapter::createSwapchain(const SwapchainDescriptor& descriptor) const {
		return createRef<VK::Swapchain>(descriptor, *surface, *device);
	}

	ref<Device> VK::GraphicsAdapter::getDevice() const {
		return device;
	}

	ref<Surface> VK::GraphicsAdapter::getSurface() const {
		return surface;
	}

	void* VK::GraphicsAdapter::getProcedure(VkInstance instance, view<char8> procedureName) const {
		static const PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = pointer_cast<PFN_vkGetInstanceProcAddr>(vulkan1Library.getProcedure("vkGetInstanceProcAddr"));

		const PFN_vkVoidFunction procedure = vkGetInstanceProcAddr(instance, &procedureName[0]);

		ADAPTER_ASSERT(procedure != nullptr, "Cannot find address for procedure \"[]\".", procedureName);

		return procedure;
	}

	void VK::GraphicsAdapter::loadBaseProcedures() const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#define LOAD_PROCEDURE(procedure) procedure = static_cast<decltype(procedure)>(getProcedure(nullptr, #procedure))

		LOAD_PROCEDURE(vkCreateInstance);
		LOAD_PROCEDURE(vkEnumerateInstanceLayerProperties);

		#undef LOAD_PROCEDURE
	}

	void VK::GraphicsAdapter::loadInstanceProcedures() const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#define LOAD_PROCEDURE(procedure) procedure = static_cast<decltype(procedure)>(getProcedure(instance->instance, #procedure))

		LOAD_PROCEDURE(vkAllocateCommandBuffers);
		LOAD_PROCEDURE(vkAllocateDescriptorSets);
		LOAD_PROCEDURE(vkAllocateMemory);
		LOAD_PROCEDURE(vkBeginCommandBuffer);
		LOAD_PROCEDURE(vkBindBufferMemory);
		LOAD_PROCEDURE(vkBindImageMemory);
		LOAD_PROCEDURE(vkCmdBindDescriptorSets);
		LOAD_PROCEDURE(vkCmdBindIndexBuffer);
		LOAD_PROCEDURE(vkCmdBindPipeline);
		LOAD_PROCEDURE(vkCmdBindVertexBuffers);
		LOAD_PROCEDURE(vkCmdCopyBuffer);
		LOAD_PROCEDURE(vkCmdCopyBufferToImage);
		LOAD_PROCEDURE(vkCmdCopyImageToBuffer);
		LOAD_PROCEDURE(vkCmdDispatch);
		LOAD_PROCEDURE(vkCmdDispatchIndirect);
		LOAD_PROCEDURE(vkCmdDraw);
		LOAD_PROCEDURE(vkCmdDrawIndexed);
		LOAD_PROCEDURE(vkCmdDrawIndexedIndirect);
		LOAD_PROCEDURE(vkCmdDrawIndirect);
		LOAD_PROCEDURE(vkCmdPipelineBarrier);
		LOAD_PROCEDURE(vkCmdSetLineWidth);
		LOAD_PROCEDURE(vkCmdSetScissor);
		LOAD_PROCEDURE(vkCmdSetViewport);
		LOAD_PROCEDURE(vkCreateBuffer);
		LOAD_PROCEDURE(vkCreateCommandPool);
		LOAD_PROCEDURE(vkCreateComputePipelines);
		LOAD_PROCEDURE(vkCreateDescriptorPool);
		LOAD_PROCEDURE(vkCreateDescriptorSetLayout);
		LOAD_PROCEDURE(vkCreateDevice);
		LOAD_PROCEDURE(vkCreateFence);
		LOAD_PROCEDURE(vkCreateGraphicsPipelines);
		LOAD_PROCEDURE(vkCreateImage);
		LOAD_PROCEDURE(vkCreateImageView);
		LOAD_PROCEDURE(vkCreatePipelineLayout);
		LOAD_PROCEDURE(vkCreateSampler);
		LOAD_PROCEDURE(vkCreateSemaphore);
		LOAD_PROCEDURE(vkCreateShaderModule);
		LOAD_PROCEDURE(vkDestroyBuffer);
		LOAD_PROCEDURE(vkDestroyCommandPool);
		LOAD_PROCEDURE(vkDestroyDescriptorPool);
		LOAD_PROCEDURE(vkDestroyDescriptorSetLayout);
		LOAD_PROCEDURE(vkDestroyDevice);
		LOAD_PROCEDURE(vkDestroyFence);
		LOAD_PROCEDURE(vkDestroyImage);
		LOAD_PROCEDURE(vkDestroyImageView);
		LOAD_PROCEDURE(vkDestroyInstance);
		LOAD_PROCEDURE(vkDestroyPipeline);
		LOAD_PROCEDURE(vkDestroyPipelineLayout);
		LOAD_PROCEDURE(vkDestroySampler);
		LOAD_PROCEDURE(vkDestroySemaphore);
		LOAD_PROCEDURE(vkDestroyShaderModule);
		LOAD_PROCEDURE(vkDeviceWaitIdle);
		LOAD_PROCEDURE(vkEndCommandBuffer);
		LOAD_PROCEDURE(vkEnumerateDeviceExtensionProperties);
		LOAD_PROCEDURE(vkEnumeratePhysicalDevices);
		LOAD_PROCEDURE(vkFreeCommandBuffers);
		LOAD_PROCEDURE(vkFreeMemory);
		LOAD_PROCEDURE(vkGetBufferMemoryRequirements);
		LOAD_PROCEDURE(vkGetDeviceQueue);
		LOAD_PROCEDURE(vkGetImageMemoryRequirements);
		LOAD_PROCEDURE(vkMapMemory);
		LOAD_PROCEDURE(vkQueueSubmit);
		LOAD_PROCEDURE(vkQueueWaitIdle);
		LOAD_PROCEDURE(vkResetCommandBuffer);
		LOAD_PROCEDURE(vkResetFences);
		LOAD_PROCEDURE(vkUnmapMemory);
		LOAD_PROCEDURE(vkUpdateDescriptorSets);
		LOAD_PROCEDURE(vkWaitForFences);

		#if VULKAN_VERSION < VULKAN_VERSION_1_1
			LOAD_PROCEDURE(vkGetPhysicalDeviceFeatures);
			LOAD_PROCEDURE(vkGetPhysicalDeviceMemoryProperties);
			LOAD_PROCEDURE(vkGetPhysicalDeviceProperties);
			LOAD_PROCEDURE(vkGetPhysicalDeviceQueueFamilyProperties);
		#endif

		#if VULKAN_VERSION < VULKAN_VERSION_1_2
			LOAD_PROCEDURE(vkCmdBeginRenderPass);
			LOAD_PROCEDURE(vkCmdEndRenderPass);
			LOAD_PROCEDURE(vkCmdNextSubpass);
			LOAD_PROCEDURE(vkCreateRenderPass);
		#endif

		#if VULKAN_VERSION < VULKAN_VERSION_1_4
			LOAD_PROCEDURE(vkCreateFramebuffer);
			LOAD_PROCEDURE(vkDestroyFramebuffer);
			LOAD_PROCEDURE(vkDestroyRenderPass);
		#endif

		#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
			LOAD_PROCEDURE(vkCreateWin32SurfaceKHR);
		#endif

		#if defined(DEBUG_ENABLE_MESSENGER)
			LOAD_PROCEDURE(vkCreateDebugUtilsMessengerEXT);
			LOAD_PROCEDURE(vkDestroyDebugUtilsMessengerEXT);
		#endif

		#if defined(VULKAN_USE_VK_EXT_debug_utils)
			LOAD_PROCEDURE(vkSetDebugUtilsObjectNameEXT);
		#endif

		#if defined(VULKAN_USE_VK_KHR_surface)
			LOAD_PROCEDURE(vkDestroySurfaceKHR);
			LOAD_PROCEDURE(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
			LOAD_PROCEDURE(vkGetPhysicalDeviceSurfaceFormatsKHR);
			LOAD_PROCEDURE(vkGetPhysicalDeviceSurfacePresentModesKHR);
			LOAD_PROCEDURE(vkGetPhysicalDeviceSurfaceSupportKHR);
		#endif

		#if defined(VULKAN_USE_VK_KHR_swapchain)
			LOAD_PROCEDURE(vkAcquireNextImageKHR);
			LOAD_PROCEDURE(vkCreateSwapchainKHR);
			LOAD_PROCEDURE(vkDestroySwapchainKHR);
			LOAD_PROCEDURE(vkGetSwapchainImagesKHR);
			LOAD_PROCEDURE(vkQueuePresentKHR);
		#endif

		#undef LOAD_PROCEDURE
	}
}