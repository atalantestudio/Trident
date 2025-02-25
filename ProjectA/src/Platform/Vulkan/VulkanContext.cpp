#include "pch.hpp"

#include "Platform/Vulkan/VulkanContext.hpp"

namespace ProjectA {
	VulkanContext::VulkanContext(GLFWwindow* window) : window(window), instance(), device(instance.object) {
		ENGINE_ASSERT(window, "Window handle is null.");
	}

	void VulkanContext::swapBuffers() const {}

	// TODO: Call before creating VulkanInstance.
	void VulkanContext::checkVulkanAvailability() {
		PROFILE_SCOPE(FUNC_SIG);

		const HMODULE vulkanDll = LoadLibrary(L"vulkan-1.dll");

		if (vulkanDll == 0) {
			throw std::runtime_error("Failed to locate Vulkan DLL.");
		}

		const PFN_vkCreateInstance vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(GetProcAddress(vulkanDll, "vkCreateInstance"));

		VkInstanceCreateInfo instanceCreateInfo{};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

		VkInstance instance;

		const VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Vulkan is not supported (failed to create VkInstance).");
		}
	}
}