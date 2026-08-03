// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace Trident {
	// TODO(tsl): Replace by a TSL shader module.
	inline ShaderModuleAsset LightmappedShaderModuleAsset() {
		ShaderModuleAsset asset{};
		asset.name = "Material/Lightmapped.frag";
		asset.stageFlags = ShaderStage::FRAGMENT;
		asset.entryPoint = "main";
		asset.bindings = {
			{
				// Diffuse
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::FRAGMENT,
				},
			},
		};
		asset.descriptorSetBindingRanges = {
			{0, 1},
		};

		return asset;
	}
}