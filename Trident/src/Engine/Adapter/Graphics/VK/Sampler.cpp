// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/AddressMode.hpp"

#if defined(DEBUG_OBJECT_NAME)
	#include "Engine/Adapter/Graphics/VK/Debug/ObjectName.hpp"
#endif

#include "Engine/Adapter/Graphics/VK/Filter.hpp"
#include "Engine/Adapter/Graphics/VK/Sampler.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::Sampler::Sampler(const Trident::Device* device) :
		Trident::Sampler(device)
	{}

	ref<Trident::Sampler> VK::createSampler(const Trident::Device* device, const SamplerDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		VkResult result;

		ref<VK::Sampler> sampler = allocateRef<VK::Sampler>(device);

		VkSamplerCreateInfo samplerCreateInfo{};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.pNext = nullptr;
		samplerCreateInfo.flags = 0;
		samplerCreateInfo.magFilter = getFilter(descriptor.magFilter);
		samplerCreateInfo.minFilter = getFilter(descriptor.minFilter);
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		samplerCreateInfo.addressModeU = getAddressMode(descriptor.addressModeU);
		samplerCreateInfo.addressModeV = getAddressMode(descriptor.addressModeV);
		samplerCreateInfo.addressModeW = getAddressMode(descriptor.addressModeW);
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.anisotropyEnable = VK_FALSE;
		samplerCreateInfo.maxAnisotropy = 1.0f;
		samplerCreateInfo.compareEnable = VK_FALSE;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = VK_LOD_CLAMP_NONE;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

		result = _device->vkCreateSampler(_device->device, &samplerCreateInfo, nullptr, &sampler->sampler);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create sampler ([]).", string_VkResult(result));

		#if defined(DEBUG_OBJECT_NAME)
			setObjectName(_device, (uint64) sampler->sampler, VK_OBJECT_TYPE_SAMPLER, descriptor.name);
		#endif

		return sampler;
	}

	void VK::destroySampler(const Trident::Device* device, const Trident::Sampler* sampler) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::Sampler* _sampler = static_cast<const VK::Sampler*>(sampler);

		_device->vkDestroySampler(_device->device, _sampler->sampler, nullptr);
	}
}