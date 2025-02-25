#include "pch.hpp"

#include "Platform/Vulkan/VulkanDevice.hpp"
#include "Platform/Vulkan/VulkanShaderModule.hpp"

namespace ProjectA {
	VulkanDevice::VulkanDevice(const VkInstance& instance) : ObjectWrapper(nullptr) {
		PROFILE_SCOPE(FUNC_SIG);

		uint32 physicalDeviceCount;

		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

		if (physicalDeviceCount == 0) {
			throw std::runtime_error("Could not find physical device(s).");
		}

		std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);

		vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

		uint32 bestPhysicalDeviceIndex = 0;
		int32 bestPhysicalDeviceScore = calculatePhysicalDeviceScore(physicalDevices[bestPhysicalDeviceIndex]);

		for (uint32 index = bestPhysicalDeviceIndex; index < physicalDevices.size(); index++) {
			const int32 score = calculatePhysicalDeviceScore(physicalDevices[index]);

			if (score > bestPhysicalDeviceScore) {
				bestPhysicalDeviceIndex = index;
				bestPhysicalDeviceScore = score;
			}
		}

		// Check the score of the best candidate.
		if (bestPhysicalDeviceScore == 0) {
			throw std::runtime_error("Could not select a suitable physical device.");
		}

		// TODO: Create device from physical device.
	}

	VulkanDevice::~VulkanDevice() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyDevice(object, nullptr);
	}

	ShaderModule VulkanDevice::createShaderModule() const {
		return VulkanShaderModule(object, VK_SHADER_STAGE_VERTEX_BIT, "");
	}

	// TODO: Physical device should be selected by a score system.
	int32 VulkanDevice::calculatePhysicalDeviceScore(const VkPhysicalDevice& physicalDevice) {
		return 1;
	}
}