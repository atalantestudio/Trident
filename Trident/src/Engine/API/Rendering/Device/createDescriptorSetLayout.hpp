// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Device.hpp"

namespace Trident {
	ref<DescriptorSetLayout> Device::createDescriptorSetLayout(const sequence<DescriptorSetLayoutBinding>& bindings) const {
		#if defined(ENABLE_GRAPHICS_ADAPTER_VALIDATION_LAYER)
			uint32 warningCount = 0;
			uint32 errorCount = 0;
			uint32 descriptorSetLayoutBindingIndex = 0;

			while (descriptorSetLayoutBindingIndex < bindings.count()) {
				const DescriptorSetLayoutBinding& descriptorSetLayoutBinding = bindings[descriptorSetLayoutBindingIndex];

				if (descriptorSetLayoutBinding.descriptorType == DescriptorType::INPUT_ATTACHMENT && descriptorSetLayoutBinding.descriptorCount > 0 && !(descriptorSetLayoutBinding.shaderStages & ShaderStage::FRAGMENT)) {
					ENGINE_ERROR("If descriptorType is INPUT_ATTACHMENT and descriptorCount is not 0, then stageFlags must be FRAGMENT.", descriptorSetLayoutBindingIndex);

					errorCount += 1;
				}

				descriptorSetLayoutBindingIndex += 1;
			}

			ENGINE_ASSERT(warningCount == 0 && errorCount == 0, "Invalid argument(s) ([] warnings and [] errors).", warningCount, errorCount);
		#endif

		return adapter->createDescriptorSetLayout(this, bindings);
	}
}