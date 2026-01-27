// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Sampler.hpp"
#include "Engine/Adapter/Graphics/VK/SamplerFilter.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::Sampler::Sampler(const SamplerDescriptor& descriptor, const VK::Device& device) :
        Trident::Sampler(descriptor),
		device(device)
	{
        PROFILE_SCOPE(FUNCTION_SIGNATURE);

		VkSamplerCreateInfo samplerCreateInfo{};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerCreateInfo.pNext = nullptr;
        samplerCreateInfo.flags = 0;
        samplerCreateInfo.magFilter = getFilter(descriptor.magFilter);
        samplerCreateInfo.minFilter = getFilter(descriptor.minFilter);
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

        const VkResult result = vkCreateSampler(device.device, &samplerCreateInfo, nullptr, &sampler);

        ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create sampler ([]).", string_VkResult(result));
	}

	VK::Sampler::~Sampler() {
        PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroySampler(device.device, sampler, nullptr);
	}
}