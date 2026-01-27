// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	extern PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
	extern PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets;
	extern PFN_vkAllocateMemory vkAllocateMemory;
	extern PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
	extern PFN_vkBindBufferMemory vkBindBufferMemory;
	extern PFN_vkBindImageMemory vkBindImageMemory;
	extern PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets;
	extern PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer;
	extern PFN_vkCmdBindPipeline vkCmdBindPipeline;
	extern PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers;
	extern PFN_vkCmdCopyBuffer vkCmdCopyBuffer;
	extern PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage;
	extern PFN_vkCmdCopyImageToBuffer vkCmdCopyImageToBuffer;
	extern PFN_vkCmdDispatch vkCmdDispatch;
	extern PFN_vkCmdDispatchIndirect vkCmdDispatchIndirect;
	extern PFN_vkCmdDraw vkCmdDraw;
	extern PFN_vkCmdDrawIndexed vkCmdDrawIndexed;
	extern PFN_vkCmdDrawIndexedIndirect vkCmdDrawIndexedIndirect;
	extern PFN_vkCmdDrawIndirect vkCmdDrawIndirect;
	extern PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
	extern PFN_vkCmdSetLineWidth vkCmdSetLineWidth;
	extern PFN_vkCmdSetScissor vkCmdSetScissor;
	extern PFN_vkCmdSetViewport vkCmdSetViewport;
	extern PFN_vkCreateBuffer vkCreateBuffer;
	extern PFN_vkCreateCommandPool vkCreateCommandPool;
	extern PFN_vkCreateComputePipelines vkCreateComputePipelines;
	extern PFN_vkCreateDescriptorPool vkCreateDescriptorPool;
	extern PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
	extern PFN_vkCreateDevice vkCreateDevice;
	extern PFN_vkCreateFence vkCreateFence;
	extern PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
	extern PFN_vkCreateImage vkCreateImage;
	extern PFN_vkCreateImageView vkCreateImageView;
	extern PFN_vkCreateInstance vkCreateInstance;
	extern PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
	extern PFN_vkCreateSampler vkCreateSampler;
	extern PFN_vkCreateSemaphore vkCreateSemaphore;
	extern PFN_vkCreateShaderModule vkCreateShaderModule;
	extern PFN_vkDestroyBuffer vkDestroyBuffer;
	extern PFN_vkDestroyCommandPool vkDestroyCommandPool;
	extern PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool;
	extern PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout;
	extern PFN_vkDestroyDevice vkDestroyDevice;
	extern PFN_vkDestroyFence vkDestroyFence;
	extern PFN_vkDestroyImage vkDestroyImage;
	extern PFN_vkDestroyImageView vkDestroyImageView;
	extern PFN_vkDestroyInstance vkDestroyInstance;
	extern PFN_vkDestroyPipeline vkDestroyPipeline;
	extern PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout;
	extern PFN_vkDestroySampler vkDestroySampler;
	extern PFN_vkDestroySemaphore vkDestroySemaphore;
	extern PFN_vkDestroyShaderModule vkDestroyShaderModule;
	extern PFN_vkDeviceWaitIdle vkDeviceWaitIdle;
	extern PFN_vkEndCommandBuffer vkEndCommandBuffer;
	extern PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
	extern PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
	extern PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
	extern PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
	extern PFN_vkFreeMemory vkFreeMemory;
	extern PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
	extern PFN_vkGetDeviceQueue vkGetDeviceQueue;
	extern PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
	extern PFN_vkMapMemory vkMapMemory;
	extern PFN_vkQueueSubmit vkQueueSubmit;
	extern PFN_vkQueueWaitIdle vkQueueWaitIdle;
	extern PFN_vkResetCommandBuffer vkResetCommandBuffer;
	extern PFN_vkResetFences vkResetFences;
	extern PFN_vkUnmapMemory vkUnmapMemory;
	extern PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets;
	extern PFN_vkWaitForFences vkWaitForFences;

	// These functions are deprecated by Vulkan 1.1.
	#if VULKAN_VERSION < VULKAN_VERSION_1_1
		extern PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
		extern PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
		extern PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
		extern PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
	#endif

	// These functions are deprecated by Vulkan 1.2.
	#if VULKAN_VERSION < VULKAN_VERSION_1_2
		extern PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass;
		extern PFN_vkCmdEndRenderPass vkCmdEndRenderPass;
		extern PFN_vkCmdNextSubpass vkCmdNextSubpass;
		extern PFN_vkCreateRenderPass vkCreateRenderPass;
	#endif

	// These functions are deprecated by Vulkan 1.4.
	#if VULKAN_VERSION < VULKAN_VERSION_1_4
		extern PFN_vkCreateFramebuffer vkCreateFramebuffer;
		extern PFN_vkDestroyFramebuffer vkDestroyFramebuffer;
		extern PFN_vkDestroyRenderPass vkDestroyRenderPass;
	#endif

	#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
		extern PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
	#endif

	#ifdef DEBUG_ENABLE_MESSENGER
		extern PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
		extern PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
	#endif

	#ifdef VULKAN_USE_VK_KHR_surface
		extern PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
		extern PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
		extern PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
		extern PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
		extern PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
	#endif

	#ifdef VULKAN_USE_VK_KHR_swapchain
		extern PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
		extern PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
		extern PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
		extern PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
		extern PFN_vkQueuePresentKHR vkQueuePresentKHR;
	#endif
}