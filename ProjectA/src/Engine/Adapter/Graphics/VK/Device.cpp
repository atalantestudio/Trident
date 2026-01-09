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
#include "Engine/Adapter/Graphics/VK/ImageLayout.hpp"
#include "Engine/Adapter/Graphics/VK/ImageView.hpp"
#include "Engine/Adapter/Graphics/VK/PhysicalDevice.hpp"
#include "Engine/Adapter/Graphics/VK/Queue.hpp"
#include "Engine/Adapter/Graphics/VK/Sampler.hpp"
#include "Engine/Adapter/Graphics/VK/Semaphore.hpp"
#include "Engine/Adapter/Graphics/VK/ShaderModule.hpp"
#include "Engine/Adapter/Graphics/VK/Surface.hpp"
#include "Engine/Adapter/Graphics/VK/Swapchain.hpp"

namespace ProjectA {
	// TODO: Client-side extension request.
	const sequence<const char8*> VK::Device::ENABLED_EXTENSION_NAMES = {
		VK_KHR_MAINTENANCE_1_EXTENSION_NAME,
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	// NOTE: ~3Mb in Release mode.
	VK::Device::Device(const VK::PhysicalDevice& physicalDevice, const VK::Surface& surface) :
		physicalDevice(physicalDevice)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		// TODO: Replace with sequence<T>.
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
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32>(ENABLED_EXTENSION_NAMES.count());
		deviceCreateInfo.ppEnabledExtensionNames = &ENABLED_EXTENSION_NAMES[0];
		deviceCreateInfo.pEnabledFeatures = &physicalDevice.features;

		const VkResult result = vkCreateDevice(physicalDevice.physicalDevice, &deviceCreateInfo, nullptr, &device);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create device ([]).", string_VkResult(result));

		queues[0] = createRef<VK::Queue>(*this, physicalDevice.graphicsQueueFamilyIndex);
		queues[1] = createRef<VK::Queue>(*this, physicalDevice.computeQueueFamilyIndex);
		queues[2] = createRef<VK::Queue>(*this, physicalDevice.transferQueueFamilyIndex);

		vkGetPhysicalDeviceMemoryProperties(physicalDevice.physicalDevice, &memoryProperties);
	}

	VK::Device::~Device() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		// Destroy the command pools before the device.
		for (const ref<VK::Queue> queue : queues) {
			queue.~shared_ptr();
		}

		for (const VkDescriptorPool descriptorPool : descriptorPools) {
			vkDestroyDescriptorPool(device, descriptorPool, nullptr);
		}

		vkDestroyDevice(device, nullptr);
	}

	ref<PhysicalDevice> VK::Device::getPhysicalDevice() const {
		return createRef<VK::PhysicalDevice>(physicalDevice);
	}

	ref<Queue> VK::Device::getQueue(QueueFamilyCapability queueFamilyCapability) const {
		return queues[static_cast<uint32>(queueFamilyCapability)];
	}

	AcquireNextImageStatus VK::Device::acquireNextImage(ref<ProjectA::Swapchain> swapchain, ref<ProjectA::Semaphore> semaphore, uint32& imageIndex) const {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VkResult result = vkAcquireNextImageKHR(
			device,
			std::static_pointer_cast<VK::Swapchain>(swapchain)->swapchain,
			max<uint64>(),
			std::static_pointer_cast<VK::Semaphore>(semaphore)->semaphore,
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
				ADAPTER_ASSERT(false, "Unhandled status.");

				return AcquireNextImageStatus::ERROR_UNHANDLED;
		}
	}

	sequence<ref<DescriptorSet>> VK::Device::allocateDescriptorSets(const sequence<ref<ProjectA::DescriptorSetLayout>>& descriptorSetLayouts) {
		VkResult result;

		const uint32 descriptorSetCount = static_cast<uint32>(descriptorSetLayouts.count());

		sequence<VkDescriptorSetLayout> vkDescriptorSetLayouts(descriptorSetCount);
		sequence<uint32> descriptorCountPerType(static_cast<uint32>(DescriptorType::count));

		for (uint32 descriptorSetLayoutIndex = 0; descriptorSetLayoutIndex < descriptorSetLayouts.count(); descriptorSetLayoutIndex += 1) {
			const ref<ProjectA::DescriptorSetLayout> descriptorSetLayout = descriptorSetLayouts[descriptorSetLayoutIndex];

			for (const DescriptorSetLayoutBinding descriptorSetLayoutBinding : descriptorSetLayout->bindings) {
				descriptorCountPerType[static_cast<uint32>(descriptorSetLayoutBinding.descriptorType)] += descriptorSetLayoutBinding.descriptorCount;
			}

			vkDescriptorSetLayouts[descriptorSetLayoutIndex] = std::static_pointer_cast<VK::DescriptorSetLayout>(descriptorSetLayout)->descriptorSetLayout;
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

		result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create descriptor pool ([]).", string_VkResult(result));

		descriptorPools.emplace_back(descriptorPool);

		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.pNext = nullptr;
		descriptorSetAllocateInfo.descriptorPool = descriptorPool;
		descriptorSetAllocateInfo.descriptorSetCount = descriptorSetCount;
		descriptorSetAllocateInfo.pSetLayouts = &vkDescriptorSetLayouts[0];

		sequence<VkDescriptorSet> vkDescriptorSets(descriptorSetCount);

		result = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &vkDescriptorSets[0]);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to allocate descriptor sets ([]).", string_VkResult(result));

		sequence<ref<ProjectA::DescriptorSet>> descriptorSets(descriptorSetLayouts.count());

		for (uint32 descriptorSetIndex = 0; descriptorSetIndex < descriptorSets.count(); descriptorSetIndex += 1) {
			const ref<ProjectA::DescriptorSetLayout> descriptorSetLayout = descriptorSetLayouts[descriptorSetIndex];
			const ref<VK::DescriptorSet> descriptorSet = createRef<VK::DescriptorSet>(vkDescriptorSets[descriptorSetIndex]);

			descriptorSet->bindings = sequence<VK::DescriptorSet::DescriptorSetBinding>(descriptorSetLayout->bindings.count());

			for (uint32 descriptorSetBindingIndex = 0; descriptorSetBindingIndex < descriptorSet->bindings.count(); descriptorSetBindingIndex += 1) {
				const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = descriptorSetLayout->bindings[descriptorSetBindingIndex];

				VK::DescriptorSet::DescriptorSetBinding& vkDescriptorSetBinding = descriptorSet->bindings[descriptorSetBindingIndex];
				vkDescriptorSetBinding.descriptorType = descriptorSetLayoutBinding.descriptorType;
			}

			descriptorSets[descriptorSetIndex] = descriptorSet;
		}

		return descriptorSets;
	}

	void VK::Device::resetFence(ref<ProjectA::Fence> fence) const {
		const VkResult result = vkResetFences(device, 1, &std::static_pointer_cast<VK::Fence>(fence)->fence);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to reset fence ([]).", string_VkResult(result));
	}

	void VK::Device::updateDescriptorSets(const sequence<WriteDescriptorSet>& writeDescriptorSets) const {
		uint64 descriptorBufferInfoCount = 0;
		uint64 descriptorImageInfoCount = 0;

		for (const WriteDescriptorSet& writeDescriptorSet : writeDescriptorSets) {
			const ref<VK::DescriptorSet> descriptorSet = std::static_pointer_cast<VK::DescriptorSet>(writeDescriptorSet.descriptorSet);

			ADAPTER_ASSERT(writeDescriptorSet.bindingIndex < descriptorSet->bindings.count(), "This descriptor set doesn't have binding index [].", writeDescriptorSet.bindingIndex);

			const VK::DescriptorSet::DescriptorSetBinding& descriptorSetBinding = descriptorSet->bindings[writeDescriptorSet.bindingIndex];

			if (isBufferDescriptorType(descriptorSetBinding.descriptorType)) {
				descriptorBufferInfoCount += writeDescriptorSet.descriptorBuffers.count();
			}
			else if (isImageDescriptorType(descriptorSetBinding.descriptorType)) {
				descriptorImageInfoCount += writeDescriptorSet.descriptorImages.count();
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
			const VK::DescriptorSet::DescriptorSetBinding& descriptorSetBinding = descriptorSet->bindings[writeDescriptorSet.bindingIndex];

			VkWriteDescriptorSet& vkWriteDescriptorSet = vkWriteDescriptorSets[writeDescriptorSetIndex];
			vkWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			vkWriteDescriptorSet.pNext = nullptr;
			vkWriteDescriptorSet.dstSet = std::static_pointer_cast<VK::DescriptorSet>(writeDescriptorSet.descriptorSet)->descriptorSet;
			vkWriteDescriptorSet.dstBinding = writeDescriptorSet.bindingIndex;
			vkWriteDescriptorSet.dstArrayElement = 0;
			vkWriteDescriptorSet.descriptorType = getDescriptorType(descriptorSetBinding.descriptorType);

			if (isBufferDescriptorType(descriptorSetBinding.descriptorType)) {
				const uint32 descriptorBufferCount = static_cast<uint32>(writeDescriptorSet.descriptorBuffers.count());

				for (uint32 descriptorBufferIndex = 0; descriptorBufferIndex < descriptorBufferCount; descriptorBufferIndex += 1) {
					const DescriptorBuffer& descriptorBuffer = writeDescriptorSet.descriptorBuffers[descriptorBufferIndex];

					VkDescriptorBufferInfo& descriptorBufferInfo = descriptorBufferInfos[descriptorBufferInfoOffset + descriptorBufferIndex];
					descriptorBufferInfo.buffer = std::static_pointer_cast<VK::Buffer>(descriptorBuffer.buffer)->buffer;
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
					const ref<VK::ImageView> imageView = std::static_pointer_cast<VK::ImageView>(descriptorImage.imageView);

					VkDescriptorImageInfo& descriptorImageInfo = descriptorImageInfos[descriptorImageInfoOffset + descriptorImageIndex];

					if (descriptorImage.sampler != nullptr) {
						descriptorImageInfo.sampler = std::static_pointer_cast<VK::Sampler>(descriptorImage.sampler)->sampler;
					}
					else {
						descriptorImageInfo.sampler = nullptr;
					}

					descriptorImageInfo.imageView = imageView->imageView;
					descriptorImageInfo.imageLayout = getImageLayout(descriptorImage.imageLayout);
				}

				vkWriteDescriptorSet.descriptorCount = descriptorImageCount;
				vkWriteDescriptorSet.pImageInfo = &descriptorImageInfos[descriptorImageInfoOffset];

				descriptorImageInfoOffset += descriptorImageCount;
			}
			else {
				ADAPTER_ASSERT(false, "The descriptor must either have a buffer type or an image type.");
			}

			vkWriteDescriptorSet.pTexelBufferView = nullptr;

			writeDescriptorSetIndex += 1;
		}

		vkUpdateDescriptorSets(device, static_cast<uint32>(vkWriteDescriptorSets.count()), &vkWriteDescriptorSets[0], 0, nullptr);
	}

	void VK::Device::waitForFence(ref<ProjectA::Fence> fence) const {
		const VkResult result = vkWaitForFences(device, 1, &std::static_pointer_cast<VK::Fence>(fence)->fence, VK_TRUE, max<uint64>());

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to wait for fence ([]).", string_VkResult(result));
	}

	void VK::Device::waitIdle() const {
		const VkResult result = vkDeviceWaitIdle(device);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to wait for device idle state ([]).", string_VkResult(result));
	}
}