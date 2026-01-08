#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Buffer.hpp"
#include "Engine/Adapter/Graphics/VK/CommandEncoder.hpp"
#include "Engine/Adapter/Graphics/VK/ComputePipeline.hpp"
#include "Engine/Adapter/Graphics/VK/DescriptorSetLayout.hpp"
#include "Engine/Adapter/Graphics/VK/Fence.hpp"
#include "Engine/Adapter/Graphics/VK/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/VK/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/VK/Image.hpp"
#include "Engine/Adapter/Graphics/VK/MemoryAllocator.hpp"
#include "Engine/Adapter/Graphics/VK/RenderPipeline.hpp"
#include "Engine/Adapter/Graphics/VK/Sampler.hpp"
#include "Engine/Adapter/Graphics/VK/Semaphore.hpp"
#include "Engine/Adapter/Graphics/VK/ShaderModule.hpp"

namespace ProjectA {
	VulkanGraphicsAdapter::VulkanGraphicsAdapter(const ProjectDescriptor& projectDescriptor, const Window& window) :
		GraphicsAdapter(window),
		vulkan1Library(Platform::openLibrary("vulkan-1.dll"))
	{
		logger = ConsoleLogger(std::cout, LogLevel::ALL, "VULKAN");

		loadBaseProcedures();

		instance = createRef<VulkanInstance>(projectDescriptor);

		loadInstanceProcedures();

		#ifdef DEBUG_ENABLE_MESSENGER
			instance->createDebugUtilsMessenger();
		#endif

		surface = createRef<VulkanSurface>(instance->object, window);
		physicalDevice = createRef<VulkanPhysicalDevice>(instance->selectPhysicalDevice());
		device = createRef<VulkanDevice>(*physicalDevice, *surface);

		surface->setPhysicalDevice(physicalDevice->object);
		surface->checkPresentQueueFamilyIndex(physicalDevice->graphicsQueueFamilyIndex);

		MemoryAllocator::initialize(*physicalDevice);
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

	ref<DescriptorSetLayout> VulkanGraphicsAdapter::createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings) const {
		return createRef<VulkanDescriptorSetLayout>(device->object, bindings);
	}

	ref<Fence> VulkanGraphicsAdapter::createFence() const {
		return createRef<VulkanFence>(device->object, true);
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
		return createRef<VulkanRenderPass>(*physicalDevice, *surface, *device, descriptor);
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

	ref<ShaderModule> VulkanGraphicsAdapter::createShaderModule(const ShaderModuleDescriptor& descriptor, ref<ShaderModuleAsset> asset) const {
		// TODO: Add support for SPIR-V shader module assets.
		ENGINE_ERROR_UNIMPLEMENTED();

		return nullptr;
	}

	ref<ShaderModule> VulkanGraphicsAdapter::createShaderModule(const ShaderModuleDescriptor& descriptor, view<char8> code) const {
		return createRef<VulkanShaderModule>(descriptor, code, device->object);
	}

	ref<Swapchain> VulkanGraphicsAdapter::createSwapchain(const SwapchainDescriptor& descriptor) const {
		return createRef<VulkanSwapchain>(*physicalDevice, *surface, *device, descriptor);
	}

	ref<Device> VulkanGraphicsAdapter::getDevice() const {
		return device;
	}

	ref<Surface> VulkanGraphicsAdapter::getSurface() const {
		return surface;
	}

	void* VulkanGraphicsAdapter::getProcedure(VkInstance instance, std::string_view procedureName) const {
		static const PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = pointer_cast<PFN_vkGetInstanceProcAddr>(vulkan1Library.getProcedure("vkGetInstanceProcAddr"));

		const PFN_vkVoidFunction procedure = vkGetInstanceProcAddr(instance, procedureName.data());

		ENGINE_ASSERT(procedure != nullptr, "Cannot find address for procedure \"[]\".", procedureName);

		return procedure;
	}

	void VulkanGraphicsAdapter::loadBaseProcedures() const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#define LOAD_PROCEDURE(procedure) procedure = static_cast<decltype(procedure)>(getProcedure(nullptr, #procedure))

		LOAD_PROCEDURE(vkCreateInstance);
		LOAD_PROCEDURE(vkEnumerateInstanceLayerProperties);

		#undef LOAD_PROCEDURE
	}

	void VulkanGraphicsAdapter::loadInstanceProcedures() const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		#define LOAD_PROCEDURE(procedure) procedure = static_cast<decltype(procedure)>(getProcedure(instance->object, #procedure))

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

		#ifdef DEBUG_ENABLE_MESSENGER
			LOAD_PROCEDURE(vkCreateDebugUtilsMessengerEXT);
			LOAD_PROCEDURE(vkDestroyDebugUtilsMessengerEXT);
		#endif

		#ifdef VULKAN_USE_VK_KHR_surface
			LOAD_PROCEDURE(vkDestroySurfaceKHR);
			LOAD_PROCEDURE(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
			LOAD_PROCEDURE(vkGetPhysicalDeviceSurfaceFormatsKHR);
			LOAD_PROCEDURE(vkGetPhysicalDeviceSurfacePresentModesKHR);
			LOAD_PROCEDURE(vkGetPhysicalDeviceSurfaceSupportKHR);
		#endif

		#ifdef VULKAN_USE_VK_KHR_swapchain
			LOAD_PROCEDURE(vkAcquireNextImageKHR);
			LOAD_PROCEDURE(vkCreateSwapchainKHR);
			LOAD_PROCEDURE(vkDestroySwapchainKHR);
			LOAD_PROCEDURE(vkGetSwapchainImagesKHR);
			LOAD_PROCEDURE(vkQueuePresentKHR);
		#endif

		#undef LOAD_PROCEDURE
	}
}