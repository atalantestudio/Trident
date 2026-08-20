// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace Trident {
	// TODO(tsl): Replace by a TSL shader module.
	inline ShaderModuleAsset PBRSeparatedAORoughnessShaderModuleAsset() {
		ShaderModuleAsset asset{};
		asset.name = "Material/PBRSeparatedAORoughness.frag";
		asset.stageFlags = ShaderStage::FRAGMENT;
		asset.entryPoint = "main";
		asset.bindings = {
			{
				// Ambient occlusion
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.stageFlags = ShaderStage::FRAGMENT,
				},

				// Roughness
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.stageFlags = ShaderStage::FRAGMENT,
				},

				// Diffuse
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.stageFlags = ShaderStage::FRAGMENT,
				},

				// Normal
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.stageFlags = ShaderStage::FRAGMENT,
				},
			},
		};
		asset.descriptorSetBindingRanges = {
			{0, 4},
		};

		return asset;
	}
}