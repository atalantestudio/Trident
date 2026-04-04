// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

#include "Engine/API/Rendering/DescriptorSetLayout.hpp"
#include "Engine/API/Rendering/DescriptorType.hpp"
#include "Engine/API/Rendering/ShaderStage.hpp"

namespace Trident {
	ref<DescriptorSetLayout> GraphicsAdapter::createDescriptorSetLayout(const Device* device, const sequence<DescriptorSetLayoutBinding>& bindings) const {
		#if defined(BUILD_MODE_DEBUG)
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

		return createDescriptorSetLayoutImplementation(device, bindings);
	}
}