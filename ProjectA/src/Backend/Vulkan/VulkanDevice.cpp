#include "pch.hpp"

#include "Backend/Vulkan/VulkanBuffer.hpp"
#include "Backend/Vulkan/VulkanCommandBuffer.hpp"
#include "Backend/Vulkan/VulkanDescriptorSet.hpp"
#include "Backend/Vulkan/VulkanDescriptorSetLayout.hpp"
#include "Backend/Vulkan/VulkanDevice.hpp"
#include "Backend/Vulkan/VulkanFence.hpp"
#include "Backend/Vulkan/VulkanSampler.hpp"
#include "Backend/Vulkan/VulkanSemaphore.hpp"
#include "Backend/Vulkan/VulkanShaderModule.hpp"
#include "Backend/Vulkan/VulkanSwapchain.hpp"

#include "Backend/Vulkan/DescriptorType.hpp"
#include "Backend/Vulkan/ImageLayout.hpp"

namespace ProjectA {
	// TODO: Client-side extension request.
	const std::vector<const char8*> VulkanDevice::enabledExtensionNames = {
		VK_KHR_MAINTENANCE_1_EXTENSION_NAME,
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	// NOTE: ~3Mb in Release mode.
	VulkanDevice::VulkanDevice(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface) :
		physicalDevice(createRef<VulkanPhysicalDevice>(physicalDevice))
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		std::vector<uint32> queueFamilyIndices = {
			physicalDevice.graphicsQueueFamilyIndex,
			physicalDevice.computeQueueFamilyIndex,
			physicalDevice.transferQueueFamilyIndex,
		};

		std::sort(queueFamilyIndices.begin(), queueFamilyIndices.end());

		queueFamilyIndices.erase(std::unique(queueFamilyIndices.begin(), queueFamilyIndices.end()), queueFamilyIndices.end());

		sequence<VkDeviceQueueCreateInfo> deviceQueueCreateInfos(queueFamilyIndices.size());

		static constexpr float32 DEFAULT_QUEUE_PRIORITY = 1.0f;

		for (uint32 queueFamilyIndexIndex = 0; queueFamilyIndexIndex < queueFamilyIndices.size(); queueFamilyIndexIndex += 1) {
			VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
			deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			deviceQueueCreateInfo.pNext = nullptr;
			deviceQueueCreateInfo.flags = 0;
			deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndices[queueFamilyIndexIndex];
			deviceQueueCreateInfo.queueCount = 1;
			deviceQueueCreateInfo.pQueuePriorities = &DEFAULT_QUEUE_PRIORITY;

			deviceQueueCreateInfos[queueFamilyIndexIndex] = deviceQueueCreateInfo;
		}

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext = nullptr;
		deviceCreateInfo.flags = 0;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(deviceQueueCreateInfos.count());
		deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfos[0];
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32>(enabledExtensionNames.size());
		deviceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames.data();
		deviceCreateInfo.pEnabledFeatures = &physicalDevice.features;

		const VkResult result = vkCreateDevice(physicalDevice.object, &deviceCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan device ([]).", string_VkResult(result));

		queues[static_cast<uint32>(QueueFamilyCapability::GRAPHICS)] = createRef<VulkanQueue>(*this, physicalDevice.graphicsQueueFamilyIndex);
		queues[static_cast<uint32>(QueueFamilyCapability::COMPUTE)] = createRef<VulkanQueue>(*this, physicalDevice.computeQueueFamilyIndex);
		queues[static_cast<uint32>(QueueFamilyCapability::TRANSFER)] = createRef<VulkanQueue>(*this, physicalDevice.transferQueueFamilyIndex);

		vkGetPhysicalDeviceMemoryProperties(physicalDevice.object, &memoryProperties);
	}

	VulkanDevice::~VulkanDevice() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		for (const ref<VulkanQueue>& queue : queues) {
			// Destroy the queue's command pool.
			queue.~shared_ptr();
		}

		for (const VkDescriptorPool descriptorPool : descriptorPools) {
			vkDestroyDescriptorPool(object, descriptorPool, nullptr);
		}

		vkDestroyDevice(object, nullptr);
	}

	ref<PhysicalDevice> VulkanDevice::getPhysicalDevice() const {
		return physicalDevice;
	}

	ref<Queue> VulkanDevice::getQueue(QueueFamilyCapability queueFamilyCapability) const {
		return queues[static_cast<uint32>(queueFamilyCapability)];
	}

	AcquireNextImageStatus VulkanDevice::acquireNextImage(const ref<Swapchain>& swapchain, const ref<Semaphore>& semaphore, uint32& imageIndex) const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VkResult result = vkAcquireNextImageKHR(
			object,
			std::static_pointer_cast<VulkanSwapchain>(swapchain)->object,
			max<uint64>(),
			std::static_pointer_cast<VulkanSemaphore>(semaphore)->object,
			VK_NULL_HANDLE,
			&imageIndex
		);

		switch (result) {
			case VK_SUCCESS:
				return AcquireNextImageStatus::SUCCESS;
			case VK_SUBOPTIMAL_KHR:
				return AcquireNextImageStatus::SUBOPTIMAL;
			case VK_ERROR_OUT_OF_DATE_KHR:
				return AcquireNextImageStatus::ERROR_OUT_OF_DATE;
			default:
				ENGINE_ASSERT(false, "Unhandled status.");

				return AcquireNextImageStatus::ERROR_UNHANDLED;
		}
	}

	sequence<ref<DescriptorSet>> VulkanDevice::allocateDescriptorSets(const sequence<ref<DescriptorSetLayout>>& descriptorSetLayouts) {
		VkResult result;

		const uint32 descriptorSetCount = static_cast<uint32>(descriptorSetLayouts.count());

		sequence<VkDescriptorSetLayout> vkDescriptorSetLayouts(descriptorSetCount);
		sequence<uint32> descriptorCountPerType(static_cast<uint32>(DescriptorType::__MAX_ENUM));

		for (uint32 descriptorSetLayoutIndex = 0; descriptorSetLayoutIndex < descriptorSetLayouts.count(); descriptorSetLayoutIndex += 1) {
			const ref<DescriptorSetLayout>& descriptorSetLayout = descriptorSetLayouts[descriptorSetLayoutIndex];

			for (const DescriptorSetLayoutBinding descriptorSetLayoutBinding : descriptorSetLayout->bindings) {
				descriptorCountPerType[static_cast<uint32>(descriptorSetLayoutBinding.descriptorType)] += descriptorSetLayoutBinding.descriptorCount;
			}

			vkDescriptorSetLayouts[descriptorSetLayoutIndex] = std::static_pointer_cast<VulkanDescriptorSetLayout>(descriptorSetLayout)->object;
		}

		uint32 descriptorPoolSizeCount = 0;

		for (const uint32 descriptorCount : descriptorCountPerType) {
			if (descriptorCount > 0) {
				descriptorPoolSizeCount += 1;
			}
		}

		sequence<VkDescriptorPoolSize> vkDescriptorPoolSizes(descriptorPoolSizeCount);

		for (uint32 descriptorPoolSizeIndex = 0, descriptorType = 0; descriptorType < descriptorCountPerType.count(); descriptorType += 1) {
			if (descriptorCountPerType[descriptorType] == 0) {
				continue;
			}

			VkDescriptorPoolSize& vkDescriptorPoolSize = vkDescriptorPoolSizes[descriptorPoolSizeIndex];
			vkDescriptorPoolSize.type = static_cast<VkDescriptorType>(descriptorType);
			vkDescriptorPoolSize.descriptorCount = descriptorCountPerType[descriptorType];

			descriptorPoolSizeIndex += 1;
		}

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.pNext = nullptr;
		descriptorPoolCreateInfo.flags = 0;
		descriptorPoolCreateInfo.maxSets = descriptorSetCount;
		descriptorPoolCreateInfo.poolSizeCount = descriptorPoolSizeCount;
		descriptorPoolCreateInfo.pPoolSizes = &vkDescriptorPoolSizes[0];

		VkDescriptorPool descriptorPool;

		result = vkCreateDescriptorPool(object, &descriptorPoolCreateInfo, nullptr, &descriptorPool);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan descriptor pool ([]).", string_VkResult(result));

		descriptorPools.emplace_back(descriptorPool);

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.pNext = nullptr;
		descriptorSetAllocateInfo.descriptorPool = descriptorPool;
		descriptorSetAllocateInfo.descriptorSetCount = descriptorSetCount;
		descriptorSetAllocateInfo.pSetLayouts = &vkDescriptorSetLayouts[0];

		sequence<VkDescriptorSet> vkDescriptorSets(descriptorSetCount);

		result = vkAllocateDescriptorSets(object, &descriptorSetAllocateInfo, &vkDescriptorSets[0]);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to allocate Vulkan descriptor sets ([]).", string_VkResult(result));

		sequence<ref<DescriptorSet>> descriptorSets(descriptorSetLayouts.count());

		for (uint32 descriptorSetIndex = 0; descriptorSetIndex < descriptorSets.count(); descriptorSetIndex += 1) {
			const ref<DescriptorSetLayout>& descriptorSetLayout = descriptorSetLayouts[descriptorSetIndex];

			ref<VulkanDescriptorSet> descriptorSet = createRef<VulkanDescriptorSet>(vkDescriptorSets[descriptorSetIndex]);
			descriptorSet->bindings = sequence<VulkanDescriptorSet::DescriptorSetBinding>(descriptorSetLayout->bindings.count());

			for (uint32 descriptorSetBindingIndex = 0; descriptorSetBindingIndex < descriptorSet->bindings.count(); descriptorSetBindingIndex += 1) {
				const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = descriptorSetLayout->bindings[descriptorSetBindingIndex];

				VulkanDescriptorSet::DescriptorSetBinding& vkDescriptorSetBinding = descriptorSet->bindings[descriptorSetBindingIndex];
				vkDescriptorSetBinding.descriptorType = descriptorSetLayoutBinding.descriptorType;
			}

			descriptorSets[descriptorSetIndex] = descriptorSet;
		}

		return descriptorSets;
	}

	void VulkanDevice::resetFence(const ref<Fence>& fence) const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VkResult result = vkResetFences(
			object,
			1,
			&std::static_pointer_cast<VulkanFence>(fence)->object
		);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to reset Vulkan fence ([]).", string_VkResult(result));
	}

	void VulkanDevice::updateDescriptorSets(const sequence<WriteDescriptorSet>& writeDescriptorSets) const {
		uint64 descriptorBufferInfoCount = 0;
		uint64 descriptorImageInfoCount = 0;

		for (const WriteDescriptorSet& writeDescriptorSet : writeDescriptorSets) {
			const ref<VulkanDescriptorSet>& descriptorSet = std::static_pointer_cast<VulkanDescriptorSet>(writeDescriptorSet.descriptorSet);

			ENGINE_ASSERT(writeDescriptorSet.bindingIndex < descriptorSet->bindings.count(), "This descriptor set doesn't have binding index [].", writeDescriptorSet.bindingIndex);

			const VulkanDescriptorSet::DescriptorSetBinding& descriptorSetBinding = descriptorSet->bindings[writeDescriptorSet.bindingIndex];

			if (isBufferDescriptorType(descriptorSetBinding.descriptorType)) {
				descriptorBufferInfoCount += writeDescriptorSet.descriptorBuffers.count();
			}
			else if (isImageDescriptorType(descriptorSetBinding.descriptorType)) {
				descriptorImageInfoCount += writeDescriptorSet.descriptorImages.count();
			}
			else {
				ENGINE_ASSERT(false, "The descriptor must either have a buffer type or an image type.");
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
			const ref<VulkanDescriptorSet>& descriptorSet = std::static_pointer_cast<VulkanDescriptorSet>(writeDescriptorSet.descriptorSet);
			const VulkanDescriptorSet::DescriptorSetBinding& descriptorSetBinding = descriptorSet->bindings[writeDescriptorSet.bindingIndex];

			VkWriteDescriptorSet& vkWriteDescriptorSet = vkWriteDescriptorSets[writeDescriptorSetIndex];
			vkWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			vkWriteDescriptorSet.pNext = nullptr;
			vkWriteDescriptorSet.dstSet = std::static_pointer_cast<VulkanDescriptorSet>(writeDescriptorSet.descriptorSet)->object;
			vkWriteDescriptorSet.dstBinding = writeDescriptorSet.bindingIndex;
			vkWriteDescriptorSet.dstArrayElement = 0;
			vkWriteDescriptorSet.descriptorType = getVkDescriptorType(descriptorSetBinding.descriptorType);

			if (isBufferDescriptorType(descriptorSetBinding.descriptorType)) {
				const uint32 descriptorBufferCount = static_cast<uint32>(writeDescriptorSet.descriptorBuffers.count());

				for (uint32 descriptorBufferIndex = 0; descriptorBufferIndex < descriptorBufferCount; descriptorBufferIndex += 1) {
					const DescriptorBuffer& descriptorBuffer = writeDescriptorSet.descriptorBuffers[descriptorBufferIndex];

					VkDescriptorBufferInfo& descriptorBufferInfo = descriptorBufferInfos[descriptorBufferInfoOffset + descriptorBufferIndex];
					descriptorBufferInfo.buffer = std::static_pointer_cast<VulkanBuffer>(descriptorBuffer.buffer)->object;
					descriptorBufferInfo.offset = descriptorBuffer.offset;
					descriptorBufferInfo.range = descriptorBuffer.size;
				}

				vkWriteDescriptorSet.descriptorCount = descriptorBufferCount;
				vkWriteDescriptorSet.pBufferInfo = &descriptorBufferInfos[descriptorBufferInfoOffset];

				descriptorBufferInfoOffset += descriptorBufferCount;
			}
			else if (isImageDescriptorType(descriptorSetBinding.descriptorType)) {
				const uint32 descriptorImageCount = static_cast<uint32>(writeDescriptorSet.descriptorImages.count());

				for (uint32 descriptorImageIndex = 0; descriptorImageIndex < descriptorImageCount; descriptorImageIndex += 1) {
					const DescriptorImage& descriptorImage = writeDescriptorSet.descriptorImages[descriptorImageIndex];
					const ref<VulkanImageView>& imageView = std::static_pointer_cast<VulkanImageView>(descriptorImage.imageView);

					VkDescriptorImageInfo& descriptorImageInfo = descriptorImageInfos[descriptorImageInfoOffset + descriptorImageIndex];

					if (descriptorImage.sampler != nullptr) {
						descriptorImageInfo.sampler = std::static_pointer_cast<VulkanSampler>(descriptorImage.sampler)->object;
					}
					else {
						descriptorImageInfo.sampler = nullptr;
					}

					descriptorImageInfo.imageView = imageView->object;
					descriptorImageInfo.imageLayout = getVkImageLayout(descriptorImage.imageLayout);
				}

				vkWriteDescriptorSet.descriptorCount = descriptorImageCount;
				vkWriteDescriptorSet.pImageInfo = &descriptorImageInfos[descriptorImageInfoOffset];

				descriptorImageInfoOffset += descriptorImageCount;
			}
			else {
				ENGINE_ASSERT(false, "The descriptor must either have a buffer type or an image type.");
			}

			vkWriteDescriptorSet.pTexelBufferView = nullptr;

			writeDescriptorSetIndex += 1;
		}

		vkUpdateDescriptorSets(
			object,
			static_cast<uint32>(vkWriteDescriptorSets.count()),
			&vkWriteDescriptorSets[0],
			0,
			nullptr
		);
	}

	void VulkanDevice::waitForFence(const ref<Fence>& fence) const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VkResult result = vkWaitForFences(
			object,
			1,
			&std::static_pointer_cast<VulkanFence>(fence)->object,
			VK_TRUE,
			max<uint64>()
		);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to wait for Vulkan fence ([]).", string_VkResult(result));
	}

	void VulkanDevice::waitIdle() const {
		const VkResult result = vkDeviceWaitIdle(object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to idle wait ([]).", string_VkResult(result));
	}
}