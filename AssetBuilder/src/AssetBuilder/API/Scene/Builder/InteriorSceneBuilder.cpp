// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Scene/Builder/InteriorSceneBuilder.hpp"

namespace Trident {
	InteriorSceneBuilder::InteriorSceneBuilder(std::filesystem::path rootOutputPath, view<char8> assetName) :
		StaticSceneBuilder(rootOutputPath, assetName)
	{
		addMaterialShader(
			"Lightmapped.frag",
			ShaderStage::FRAGMENT,
			"main",
			{
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::FRAGMENT,
				},
				{
					.descriptorType = DescriptorType::COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.shaderStages = ShaderStage::FRAGMENT,
				},
			}
		);

		addMaterial(100, 0);
	}

	void InteriorSceneBuilder::addRoom(const Room& room) {
		for (const StaticSurface& surface : room.surfaces) {
			addSurface(surface);
		}
	}
}