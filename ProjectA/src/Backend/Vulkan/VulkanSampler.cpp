#include "pch.hpp"

#include "Backend/Vulkan/VulkanSampler.hpp"

#include "Backend/Vulkan/SamplerFilter.hpp"

namespace ProjectA {
	VulkanSampler::VulkanSampler(VkDevice device, const SamplerDescriptor& descriptor) :
		device(device)
	{
        PROFILE_SCOPE(FUNC_SIG);

		VkSamplerCreateInfo samplerCreateInfo{};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.pNext = nullptr;
        samplerCreateInfo.flags = 0;
        samplerCreateInfo.magFilter = getVkFilter(descriptor.magFilter);
        samplerCreateInfo.minFilter = getVkFilter(descriptor.minFilter);
        samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerCreateInfo.mipLodBias = 0.0f;
        samplerCreateInfo.anisotropyEnable = VK_FALSE;
        samplerCreateInfo.maxAnisotropy = 1.0f;
        samplerCreateInfo.compareEnable = VK_FALSE;
        samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
        samplerCreateInfo.minLod = 0;
        samplerCreateInfo.maxLod = 0;
        samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

        const VkResult result = vkCreateSampler(device, &samplerCreateInfo, nullptr, &object);

        ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan sampler (code: []).", result);
	}

	VulkanSampler::~VulkanSampler() {
        PROFILE_SCOPE(FUNC_SIG);

		vkDestroySampler(device, object, nullptr);
	}
}