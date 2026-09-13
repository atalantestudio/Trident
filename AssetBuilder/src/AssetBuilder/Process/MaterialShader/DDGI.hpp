// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace Trident {
	// TODO(tsl): Replace by a TSL shader module.
	inline ShaderModuleAsset DDGIShaderModuleAsset() {
		ShaderModuleAsset asset{};
		asset.identifier = 404;
		asset.version = {0, 0, 0};
		asset.name = "Material/DDGI.frag";
		asset.stageFlags = ShaderStage::FRAGMENT;
		asset.entryPoint = "main";
		asset.bindings = {
			{
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
			{0, 2},
		};
		asset.geometryType = GeometryType::STATIC;

		return asset;
	}
}