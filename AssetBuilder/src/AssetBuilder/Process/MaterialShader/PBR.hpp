// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace Trident {
	// TODO(tsl): Replace by a TSL shader module.
	inline ShaderModuleAsset PBRShaderModuleAsset() {
		ShaderModuleAsset asset{};
		asset.identifier = 402;
		asset.version = {0, 0, 0};
		asset.name = "Material/PBR.frag";
		asset.stageFlags = ShaderStage::FRAGMENT;
		asset.entryPoint = "main";
		asset.bindings = {
			{
				// Ambient occlusion/metallic/roughness
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
			{0, 3},
		};
		asset.geometryType = GeometryType::DYNAMIC;

		return asset;
	}
}