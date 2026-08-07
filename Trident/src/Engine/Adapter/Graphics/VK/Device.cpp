// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Buffer.hpp"
#include "Engine/Adapter/Graphics/VK/CommandBuffer.hpp"
#include "Engine/Adapter/Graphics/VK/DescriptorSet.hpp"
#include "Engine/Adapter/Graphics/VK/DescriptorSetLayout.hpp"
#include "Engine/Adapter/Graphics/VK/DescriptorType.hpp"
#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/Fence.hpp"
#include "Engine/Adapter/Graphics/VK/GraphicsAdapter.hpp"
#include "Engine/Adapter/Graphics/VK/ImageLayout.hpp"
#include "Engine/Adapter/Graphics/VK/ImageView.hpp"
#include "Engine/Adapter/Graphics/VK/Instance.hpp"
#include "Engine/Adapter/Graphics/VK/PhysicalDevice.hpp"
#include "Engine/Adapter/Graphics/VK/Queue.hpp"
#include "Engine/Adapter/Graphics/VK/Sampler.hpp"
#include "Engine/Adapter/Graphics/VK/Semaphore.hpp"
#include "Engine/Adapter/Graphics/VK/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/VK/Status.hpp"
#include "Engine/Adapter/Graphics/VK/Swapchain.hpp"

namespace Trident {
	VK::Device::Device(const Trident::GraphicsAdapter* adapter, const Trident::PhysicalDevice* physicalDevice) :
		Trident::Device(adapter, physicalDevice)
	{}

	VK::Device::~Device() {
		// TODO(graphics/device/free-descriptor-sets): Only if the pool has been created with the VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT flag.
		/* result = vkFreeDescriptorSets(_device->device, descriptorPool, static_cast<uint32>(descriptorSets.count()), &descriptorSets[0]);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to free descriptor sets ([]).", string_VkResult(result)); */

		for (const VkDescriptorPool descriptorPool : descriptorPools) {
			vkDestroyDescriptorPool(device, descriptorPool, nullptr);
		}
	}

	void VK::Device::loadProcedures() {
		#define LOAD_DEVICE_PROCEDURE(procedure) procedure = loadDeviceProcedure<decltype(procedure)>(device, #procedure)

		LOAD_DEVICE_PROCEDURE(vkAcquireNextImageKHR);
		LOAD_DEVICE_PROCEDURE(vkAllocateCommandBuffers);
		LOAD_DEVICE_PROCEDURE(vkAllocateDescriptorSets);
		LOAD_DEVICE_PROCEDURE(vkAllocateMemory);
		LOAD_DEVICE_PROCEDURE(vkBindBufferMemory);
		LOAD_DEVICE_PROCEDURE(vkBindImageMemory);
		LOAD_DEVICE_PROCEDURE(vkCreateBuffer);
		LOAD_DEVICE_PROCEDURE(vkCreateCommandPool);
		LOAD_DEVICE_PROCEDURE(vkCreateComputePipelines);
		LOAD_DEVICE_PROCEDURE(vkCreateDescriptorPool);
		LOAD_DEVICE_PROCEDURE(vkCreateDescriptorSetLayout);
		LOAD_DEVICE_PROCEDURE(vkCreateFence);
		LOAD_DEVICE_PROCEDURE(vkCreateFramebuffer);
		LOAD_DEVICE_PROCEDURE(vkCreateGraphicsPipelines);
		LOAD_DEVICE_PROCEDURE(vkCreateImage);
		LOAD_DEVICE_PROCEDURE(vkCreateImageView);
		LOAD_DEVICE_PROCEDURE(vkCreatePipelineLayout);
		LOAD_DEVICE_PROCEDURE(vkCreateRenderPass);
		LOAD_DEVICE_PROCEDURE(vkCreateSampler);
		LOAD_DEVICE_PROCEDURE(vkCreateSemaphore);
		LOAD_DEVICE_PROCEDURE(vkCreateShaderModule);
		LOAD_DEVICE_PROCEDURE(vkCreateSwapchainKHR);
		LOAD_DEVICE_PROCEDURE(vkDestroyBuffer);
		LOAD_DEVICE_PROCEDURE(vkDestroyCommandPool);
		LOAD_DEVICE_PROCEDURE(vkDestroyDescriptorPool);
		LOAD_DEVICE_PROCEDURE(vkDestroyDescriptorSetLayout);
		LOAD_DEVICE_PROCEDURE(vkDestroyFence);
		LOAD_DEVICE_PROCEDURE(vkDestroyFramebuffer);
		LOAD_DEVICE_PROCEDURE(vkDestroyImage);
		LOAD_DEVICE_PROCEDURE(vkDestroyImageView);
		LOAD_DEVICE_PROCEDURE(vkDestroyPipeline);
		LOAD_DEVICE_PROCEDURE(vkDestroyPipelineLayout);
		LOAD_DEVICE_PROCEDURE(vkDestroyRenderPass);
		LOAD_DEVICE_PROCEDURE(vkDestroySampler);
		LOAD_DEVICE_PROCEDURE(vkDestroySemaphore);
		LOAD_DEVICE_PROCEDURE(vkDestroyShaderModule);
		LOAD_DEVICE_PROCEDURE(vkDestroySwapchainKHR);
		LOAD_DEVICE_PROCEDURE(vkDeviceWaitIdle);
		LOAD_DEVICE_PROCEDURE(vkFreeCommandBuffers);
		LOAD_DEVICE_PROCEDURE(vkFreeMemory);
		LOAD_DEVICE_PROCEDURE(vkGetDeviceQueue);
		LOAD_DEVICE_PROCEDURE(vkGetBufferMemoryRequirements);
		LOAD_DEVICE_PROCEDURE(vkGetImageMemoryRequirements);
		LOAD_DEVICE_PROCEDURE(vkGetSwapchainImagesKHR);
		LOAD_DEVICE_PROCEDURE(vkMapMemory);
		LOAD_DEVICE_PROCEDURE(vkResetFences);

		#if defined(BUILD_MODE_DEBUG)
			LOAD_DEVICE_PROCEDURE(vkSetDebugUtilsObjectNameEXT);
		#endif

		LOAD_DEVICE_PROCEDURE(vkUnmapMemory);
		LOAD_DEVICE_PROCEDURE(vkUpdateDescriptorSets);
		LOAD_DEVICE_PROCEDURE(vkWaitForFences);

		#undef LOAD_DEVICE_PROCEDURE
	}

	AcquireNextImageStatus VK::acquireNextImage(const Trident::Device* device, const Trident::Swapchain* swapchain, const Trident::Semaphore* semaphore, uint32& imageIndex) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::Swapchain* _swapchain = static_cast<const VK::Swapchain*>(swapchain);
		const VK::Semaphore* _semaphore = static_cast<const VK::Semaphore*>(semaphore);
		VkResult result;

		result = _device->vkAcquireNextImageKHR(
			_device->device,
			_swapchain->swapchain,
			max<uint64>(),
			_semaphore->semaphore,
			VK_NULL_HANDLE,
			&imageIndex
		);

		return acquireNextImageStatusFromResult(result);
	}

	sequence<ref<Trident::DescriptorSet>> VK::allocateDescriptorSets(Trident::Device* device, view<ref<Trident::DescriptorSetLayout>> descriptorSetLayouts) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		VK::Device* _device = static_cast<VK::Device*>(device);
		const uint32 descriptorSetCount = static_cast<uint32>(descriptorSetLayouts.count());

		sequence<VkDescriptorSetLayout> vkDescriptorSetLayouts(descriptorSetCount);
		sequence<uint32> descriptorCountPerType(static_cast<uint32>(DescriptorType::count));
		uint32 descriptorSetLayoutIndex = 0;
		VkResult result;

		while (descriptorSetLayoutIndex < descriptorSetLayouts.count()) {
			const ref<Trident::DescriptorSetLayout> descriptorSetLayout = descriptorSetLayouts[descriptorSetLayoutIndex];

			for (const DescriptorSetLayoutBinding descriptorSetLayoutBinding : descriptorSetLayout->bindings) {
				descriptorCountPerType[static_cast<uint32>(descriptorSetLayoutBinding.descriptorType)] += descriptorSetLayoutBinding.descriptorCount;
			}

			vkDescriptorSetLayouts[descriptorSetLayoutIndex] = std::static_pointer_cast<VK::DescriptorSetLayout>(descriptorSetLayout)->descriptorSetLayout;

			descriptorSetLayoutIndex += 1;
		}

		uint32 descriptorPoolSizeCount = 0;

		for (const uint32 descriptorCount : descriptorCountPerType) {
			if (descriptorCount > 0) {
				descriptorPoolSizeCount += 1;
			}
		}

		sequence<VkDescriptorPoolSize> vkDescriptorPoolSizes(descriptorPoolSizeCount);

		uint32 descriptorPoolSizeIndex = 0;
		uint32 descriptorType = 0;

		while (descriptorType < descriptorCountPerType.count()) {
			if (descriptorCountPerType[descriptorType] == 0) {
				descriptorType += 1;

				continue;
			}

			VkDescriptorPoolSize& vkDescriptorPoolSize = vkDescriptorPoolSizes[descriptorPoolSizeIndex];
			vkDescriptorPoolSize.type = getDescriptorType(static_cast<DescriptorType>(descriptorType));
			vkDescriptorPoolSize.descriptorCount = descriptorCountPerType[descriptorType];

			descriptorPoolSizeIndex += 1;
			descriptorType += 1;
		}

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.pNext = nullptr;
		descriptorPoolCreateInfo.flags = 0;
		descriptorPoolCreateInfo.maxSets = descriptorSetCount;
		descriptorPoolCreateInfo.poolSizeCount = descriptorPoolSizeCount;
		descriptorPoolCreateInfo.pPoolSizes = &vkDescriptorPoolSizes[0];

		VkDescriptorPool descriptorPool;

		result = _device->vkCreateDescriptorPool(_device->device, &descriptorPoolCreateInfo, nullptr, &descriptorPool);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create descriptor pool ([]).", string_VkResult(result));

		_device->descriptorPools.emplace_back(descriptorPool);

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.pNext = nullptr;
		descriptorSetAllocateInfo.descriptorPool = descriptorPool;
		descriptorSetAllocateInfo.descriptorSetCount = descriptorSetCount;
		descriptorSetAllocateInfo.pSetLayouts = &vkDescriptorSetLayouts[0];

		sequence<VkDescriptorSet> vkDescriptorSets(descriptorSetCount);

		result = _device->vkAllocateDescriptorSets(_device->device, &descriptorSetAllocateInfo, &vkDescriptorSets[0]);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to allocate descriptor sets ([]).", string_VkResult(result));

		sequence<ref<Trident::DescriptorSet>> descriptorSets(descriptorSetLayouts.count());
		uint32 descriptorSetIndex = 0;
		uint32 descriptorSetBindingIndex;

		while (descriptorSetIndex < descriptorSets.count()) {
			const ref<Trident::DescriptorSetLayout> descriptorSetLayout = descriptorSetLayouts[descriptorSetIndex];
			const ref<VK::DescriptorSet> descriptorSet = createRef<VK::DescriptorSet>(vkDescriptorSets[descriptorSetIndex]);

			descriptorSet->bindings = sequence<DescriptorSetLayoutBinding>(descriptorSetLayout->bindings.count());
			descriptorSetBindingIndex = 0;

			std::copy(descriptorSetLayout->bindings.begin(), descriptorSetLayout->bindings.end(), &descriptorSet->bindings[0]);

			descriptorSets[descriptorSetIndex] = descriptorSet;
			descriptorSetIndex += 1;
		}

		return descriptorSets;
	}

	ref<Trident::Device> VK::createDevice(const Trident::GraphicsAdapter* adapter, const Trident::PhysicalDevice* physicalDevice) {
		static constexpr float32 DEFAULT_QUEUE_PRIORITY = 1.0f;

		// TODO: Expose to the client side.
		static const sequence<const char8*> ENABLED_EXTENSION_NAMES = {
			VK_KHR_MAINTENANCE_1_EXTENSION_NAME,
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		};

		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::PhysicalDevice* _physicalDevice = static_cast<const VK::PhysicalDevice*>(physicalDevice);
		VkResult result;

		ref<VK::Device> device = createRef<VK::Device>(adapter, physicalDevice);

		// TODO: Replace with sequence<T>.
		std::vector<uint32> queueFamilyIndices = {
			_physicalDevice->graphicsQueueFamilyIndex,
			_physicalDevice->computeQueueFamilyIndex,
			_physicalDevice->transferQueueFamilyIndex,
		};

		std::sort(queueFamilyIndices.begin(), queueFamilyIndices.end());

		queueFamilyIndices.erase(std::unique(queueFamilyIndices.begin(), queueFamilyIndices.end()), queueFamilyIndices.end());

		sequence<VkDeviceQueueCreateInfo> deviceQueueCreateInfos(queueFamilyIndices.size());
		uint32 queueFamilyIndexIndex = 0;

		while (queueFamilyIndexIndex < queueFamilyIndices.size()) {
			VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
			deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			deviceQueueCreateInfo.pNext = nullptr;
			deviceQueueCreateInfo.flags = 0;
			deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndices[queueFamilyIndexIndex];
			deviceQueueCreateInfo.queueCount = 1;
			deviceQueueCreateInfo.pQueuePriorities = &DEFAULT_QUEUE_PRIORITY;

			deviceQueueCreateInfos[queueFamilyIndexIndex] = deviceQueueCreateInfo;

			queueFamilyIndexIndex += 1;
		}

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext = nullptr;
		deviceCreateInfo.flags = 0;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(deviceQueueCreateInfos.count());
		deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfos[0];
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32>(ENABLED_EXTENSION_NAMES.count());
		deviceCreateInfo.ppEnabledExtensionNames = &ENABLED_EXTENSION_NAMES[0];
		deviceCreateInfo.pEnabledFeatures = &_physicalDevice->features;

		result = _physicalDevice->instance->vkCreateDevice(_physicalDevice->physicalDevice, &deviceCreateInfo, nullptr, &device->device);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create device ([]).", string_VkResult(result));

		device->loadProcedures();

		const Trident::Device* _device = pointer_cast<const Trident::Device*>(device.get());

		device->queues[0] = createQueue(_device, _physicalDevice->graphicsQueueFamilyIndex, QueueFamilyCapability::GRAPHICS);
		device->queues[1] = createQueue(_device, _physicalDevice->computeQueueFamilyIndex, QueueFamilyCapability::COMPUTE);
		device->queues[2] = createQueue(_device, _physicalDevice->transferQueueFamilyIndex, QueueFamilyCapability::TRANSFER);

		return device;
	}

	void VK::destroyDevice(const Trident::Device* device) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		static PFN_vkDestroyDevice vkDestroyDevice = loadDeviceProcedure<PFN_vkDestroyDevice>(static_cast<const VK::Device*>(device)->device, "vkDestroyDevice");

		const VK::Device* _device = static_cast<const VK::Device*>(device);

		for (const ref<Trident::Queue>& queue : _device->queues) {
			queue.~shared_ptr();
		}

		vkDestroyDevice(_device->device, nullptr);
	}

	ref<Trident::Queue> VK::getQueue(const Trident::Device* device, QueueFamilyCapability queueFamilyCapability) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);

		return _device->queues[static_cast<uint32>(queueFamilyCapability)];
	}

	void VK::resetFence(const Trident::Device* device, const Trident::Fence* fence) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::Fence* _fence = static_cast<const VK::Fence*>(fence);

		const VkResult result = _device->vkResetFences(_device->device, 1, &_fence->fence);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to reset fence ([]).", string_VkResult(result));
	}

	void VK::updateDescriptorSets(const Trident::Device* device, view<WriteDescriptorSet> writeDescriptorSets) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);

		uint64 descriptorBufferInfoCount = 0;
		uint64 descriptorImageInfoCount = 0;

		for (const WriteDescriptorSet& writeDescriptorSet : writeDescriptorSets) {
			const ref<VK::DescriptorSet> descriptorSet = std::static_pointer_cast<VK::DescriptorSet>(writeDescriptorSet.descriptorSet);

			ADAPTER_ASSERT(writeDescriptorSet.bindingIndex < descriptorSet->bindings.count(), "The write descriptor set uses invalid binding index [].", writeDescriptorSet.bindingIndex);

			const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = descriptorSet->bindings[writeDescriptorSet.bindingIndex];

			if (isBufferDescriptorType(descriptorSetLayoutBinding.descriptorType)) {
				descriptorBufferInfoCount += descriptorSetLayoutBinding.descriptorCount;
			}
			else if (isImageDescriptorType(descriptorSetLayoutBinding.descriptorType)) {
				descriptorImageInfoCount += descriptorSetLayoutBinding.descriptorCount;
			}
			else {
				ADAPTER_ASSERT(false, "The descriptor must either have a buffer type or an image type.");
			}
		}

		sequence<VkWriteDescriptorSet> vkWriteDescriptorSets(writeDescriptorSets.count());
		sequence<VkDescriptorBufferInfo> descriptorBufferInfos(descriptorBufferInfoCount);
		sequence<VkDescriptorImageInfo> descriptorImageInfos(descriptorImageInfoCount);

		uint32 writeDescriptorSetIndex = 0;
		uint32 descriptorBufferInfoOffset = 0;
		uint32 descriptorImageInfoOffset = 0;

		while (writeDescriptorSetIndex < writeDescriptorSets.count()) {
			const WriteDescriptorSet& writeDescriptorSet = writeDescriptorSets[writeDescriptorSetIndex];
			const ref<VK::DescriptorSet> descriptorSet = std::static_pointer_cast<VK::DescriptorSet>(writeDescriptorSet.descriptorSet);
			const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = descriptorSet->bindings[writeDescriptorSet.bindingIndex];

			VkWriteDescriptorSet& vkWriteDescriptorSet = vkWriteDescriptorSets[writeDescriptorSetIndex];
			vkWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			vkWriteDescriptorSet.pNext = nullptr;
			vkWriteDescriptorSet.dstSet = std::static_pointer_cast<VK::DescriptorSet>(writeDescriptorSet.descriptorSet)->descriptorSet;
			vkWriteDescriptorSet.dstBinding = writeDescriptorSet.bindingIndex;
			vkWriteDescriptorSet.dstArrayElement = 0;
			vkWriteDescriptorSet.descriptorType = getDescriptorType(descriptorSetLayoutBinding.descriptorType);

			if (isBufferDescriptorType(descriptorSetLayoutBinding.descriptorType)) {
				uint32 descriptorBufferIndex = 0;

				while (descriptorBufferIndex < descriptorSetLayoutBinding.descriptorCount) {
					const ref<BaseBufferView> bufferView = writeDescriptorSet.descriptorBufferViews[descriptorBufferIndex];

					VkDescriptorBufferInfo& descriptorBufferInfo = descriptorBufferInfos[descriptorBufferInfoOffset + descriptorBufferIndex];
					descriptorBufferInfo.buffer = static_cast<const VK::Buffer*>(bufferView->getBuffer())->buffer;
					descriptorBufferInfo.offset = bufferView->getOffset();
					descriptorBufferInfo.range = bufferView->getSize();

					descriptorBufferIndex += 1;
				}

				vkWriteDescriptorSet.pBufferInfo = &descriptorBufferInfos[descriptorBufferInfoOffset];

				descriptorBufferInfoOffset += descriptorSetLayoutBinding.descriptorCount;
			}
			else if (isImageDescriptorType(descriptorSetLayoutBinding.descriptorType)) {
				uint32 descriptorImageIndex = 0;

				while (descriptorImageIndex < descriptorSetLayoutBinding.descriptorCount) {
					const DescriptorImage& descriptorImage = writeDescriptorSet.descriptorImages[descriptorImageIndex];
					const ref<VK::ImageView> imageView = std::static_pointer_cast<VK::ImageView>(descriptorImage.imageView);

					VkDescriptorImageInfo& descriptorImageInfo = descriptorImageInfos[descriptorImageInfoOffset + descriptorImageIndex];
					descriptorImageInfo.sampler = nullptr;

					if (descriptorImage.sampler != nullptr) {
						descriptorImageInfo.sampler = std::static_pointer_cast<VK::Sampler>(descriptorImage.sampler)->sampler;
					}

					descriptorImageInfo.imageView = imageView->imageView;
					descriptorImageInfo.imageLayout = getImageLayout(descriptorImage.imageLayout);

					descriptorImageIndex += 1;
				}

				vkWriteDescriptorSet.pImageInfo = &descriptorImageInfos[descriptorImageInfoOffset];

				descriptorImageInfoOffset += descriptorSetLayoutBinding.descriptorCount;
			}
			else {
				ADAPTER_ASSERT(false, "The descriptor must either have a buffer type or an image type.");
			}

			vkWriteDescriptorSet.descriptorCount = descriptorSetLayoutBinding.descriptorCount;
			vkWriteDescriptorSet.pTexelBufferView = nullptr;

			writeDescriptorSetIndex += 1;
		}

		_device->vkUpdateDescriptorSets(_device->device, static_cast<uint32>(vkWriteDescriptorSets.count()), &vkWriteDescriptorSets[0], 0, nullptr);
	}

	void VK::waitForFence(const Trident::Device* device, Trident::Fence* fence) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::Fence* _fence = static_cast<const VK::Fence*>(fence);
		VkResult result;

		result = _device->vkWaitForFences(_device->device, 1, &_fence->fence, VK_TRUE, max<uint64>());

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to wait for fence ([]).", string_VkResult(result));
	}

	void VK::waitIdle(const Trident::Device* device) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		VkResult result;

		result = _device->vkDeviceWaitIdle(_device->device);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to wait for device idle state ([]).", string_VkResult(result));
	}
}