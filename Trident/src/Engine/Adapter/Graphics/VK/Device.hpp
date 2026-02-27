// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Device.hpp"

namespace Trident {
	class VK::Device final : public Trident::Device {
		public:
			explicit Device(const Trident::GraphicsAdapter* adapter, const Trident::PhysicalDevice* physicalDevice);

		public:
			VkDevice device;
			ref<VK::Queue> queues[3];

			// TODO: Move in VK::MemoryAllocator.
			std::vector<VkDescriptorPool> descriptorPools;

			PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
			PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
			PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets;
			PFN_vkCreateBuffer vkCreateBuffer;
			PFN_vkCreateCommandPool vkCreateCommandPool;
			PFN_vkCreateComputePipelines vkCreateComputePipelines;
			PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
			PFN_vkCreateFence vkCreateFence;
			PFN_vkCreateFramebuffer vkCreateFramebuffer;
			PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
			PFN_vkCreateSemaphore vkCreateSemaphore;
			PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
			PFN_vkDestroyBuffer vkDestroyBuffer;
			PFN_vkDestroyCommandPool vkDestroyCommandPool;
			PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool;
			PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout;

			// TODO
			PFN_vkDestroyDevice vkDestroyDevice;

			PFN_vkDestroyFence vkDestroyFence;
			PFN_vkDestroyFramebuffer vkDestroyFramebuffer;
			PFN_vkDestroyPipeline vkDestroyPipeline;
			PFN_vkDestroySemaphore vkDestroySemaphore;
			PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
			PFN_vkDeviceWaitIdle vkDeviceWaitIdle;
			PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
			PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
			PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
			PFN_vkResetFences vkResetFences;
			PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets;
			PFN_vkWaitForFences vkWaitForFences;

			#if defined(BUILD_MODE_DEBUG)
				PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
			#endif
	};

	namespace VK {
		AcquireNextImageStatus acquireNextImage(const Trident::Device* device, const Trident::Swapchain* swapchain, const Semaphore* semaphore, uint32& imageIndex);

		sequence<ref<Trident::DescriptorSet>> allocateDescriptorSets(const Trident::Device* device, view<ref<DescriptorSetLayout>> descriptorSetLayouts);

		ref<Trident::Device> createDevice(const Trident::GraphicsAdapter* adapter, const Trident::PhysicalDevice* physicalDevice);

		void destroyDevice(const Trident::Device* device);

		ref<Trident::Queue> getQueue(const Trident::Device* device, QueueFamilyCapability queueFamilyCapability);

		void resetFence(const Trident::Device* device, const Trident::Fence* fence);

		void updateDescriptorSets(const Trident::Device* device, view<WriteDescriptorSet> writeDescriptorSets);

		void waitForFence(const Trident::Device* device, const Trident::Fence* fence);

		void waitIdle(const Trident::Device* device);
	}
}