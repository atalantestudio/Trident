// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Device.hpp"

namespace Trident {
	class VK::Device final : public Trident::Device {
		public:
			explicit Device(const Trident::GraphicsAdapter* adapter, const Trident::PhysicalDevice* physicalDevice);

			~Device();

			void loadProcedures();

		public:
			VkDevice device;
			ref<Trident::Queue> queues[3];

			// TODO: Move in VK::MemoryAllocator.
			std::vector<VkDescriptorPool> descriptorPools;

			PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
			PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
			PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets;
			PFN_vkAllocateMemory vkAllocateMemory;
			PFN_vkBindBufferMemory vkBindBufferMemory;
			PFN_vkBindImageMemory vkBindImageMemory;
			PFN_vkCreateBuffer vkCreateBuffer;
			PFN_vkCreateCommandPool vkCreateCommandPool;
			PFN_vkCreateComputePipelines vkCreateComputePipelines;
			PFN_vkCreateDescriptorPool vkCreateDescriptorPool;
			PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
			PFN_vkCreateFence vkCreateFence;
			PFN_vkCreateFramebuffer vkCreateFramebuffer;
			PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
			PFN_vkCreateImage vkCreateImage;
			PFN_vkCreateImageView vkCreateImageView;
			PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
			PFN_vkCreateRenderPass vkCreateRenderPass;
			PFN_vkCreateSampler vkCreateSampler;
			PFN_vkCreateSemaphore vkCreateSemaphore;
			PFN_vkCreateShaderModule vkCreateShaderModule;
			PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
			PFN_vkDestroyBuffer vkDestroyBuffer;
			PFN_vkDestroyCommandPool vkDestroyCommandPool;
			PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool;
			PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout;
			PFN_vkDestroyFence vkDestroyFence;
			PFN_vkDestroyFramebuffer vkDestroyFramebuffer;
			PFN_vkDestroyImage vkDestroyImage;
			PFN_vkDestroyImageView vkDestroyImageView;
			PFN_vkDestroyPipeline vkDestroyPipeline;
			PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout;
			PFN_vkDestroyRenderPass vkDestroyRenderPass;
			PFN_vkDestroySampler vkDestroySampler;
			PFN_vkDestroySemaphore vkDestroySemaphore;
			PFN_vkDestroyShaderModule vkDestroyShaderModule;
			PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
			PFN_vkDeviceWaitIdle vkDeviceWaitIdle;
			PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
			PFN_vkFreeMemory vkFreeMemory;
			PFN_vkGetDeviceQueue vkGetDeviceQueue;
			PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
			PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
			PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
			PFN_vkMapMemory vkMapMemory;
			PFN_vkResetFences vkResetFences;

			#if defined(BUILD_MODE_DEBUG)
				PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
			#endif

			PFN_vkUnmapMemory vkUnmapMemory;
			PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets;
			PFN_vkWaitForFences vkWaitForFences;
	};

	namespace VK {
		AcquireNextImageStatus acquireNextImage(const Trident::Device* device, const Trident::Swapchain* swapchain, const Trident::Semaphore* semaphore, uint32& imageIndex);

		sequence<ref<Trident::DescriptorSet>> allocateDescriptorSets(Trident::Device* device, view<ref<Trident::DescriptorSetLayout>> descriptorSetLayouts);

		ref<Trident::Device> createDevice(const Trident::GraphicsAdapter* adapter, const Trident::PhysicalDevice* physicalDevice);

		void destroyDevice(const Trident::Device* device);

		ref<Trident::Queue> getQueue(const Trident::Device* device, QueueFamilyCapability queueFamilyCapability);

		void resetFence(const Trident::Device* device, const Trident::Fence* fence);

		void updateDescriptorSets(const Trident::Device* device, view<WriteDescriptorSet> writeDescriptorSets);

		void waitForFence(const Trident::Device* device, Trident::Fence* fence);

		void waitIdle(const Trident::Device* device);
	}
}