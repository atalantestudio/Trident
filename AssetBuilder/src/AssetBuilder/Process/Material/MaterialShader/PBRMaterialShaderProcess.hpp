// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/MaterialShaderProcess.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident {
	struct PBRMaterialDescriptor {
		view<char8> armTexturePath;
		view<char8> diffuseTexturePath;

		/// Must be specified in GL format.
		view<char8> normalTexturePath;
	};

	class PBRMaterialShaderProcess : public MaterialShaderProcess {
		protected:
			explicit PBRMaterialShaderProcess(AssetBuilderLibrary* assetLibrary, const PBRMaterialDescriptor& descriptor) :
				MaterialShaderProcess(assetLibrary),
				descriptor(descriptor)
			{}

			MaterialAsset createMaterialAsset(std::filesystem::path rootOutputPath) const final override {
				AssetLibrary assetLibrary(rootOutputPath);

				MaterialAsset asset{};

				// TODO(asset-builder/material): Find another way of specifying the shader index that's not relative to the current scene.
				asset.shaderIndex = 1;

				asset.descriptorOffset = 0;
				asset.descriptorImages = {
					{
						.imageAsset = assetLibrary.getAsset<ImageAsset>(descriptor.armTexturePath),
					},
					{
						.imageAsset = assetLibrary.getAsset<ImageAsset>(descriptor.diffuseTexturePath),
					},
					{
						.imageAsset = assetLibrary.getAsset<ImageAsset>(descriptor.normalTexturePath),
					},
				};

				return asset;
			}

		private:
			PBRMaterialDescriptor descriptor;
	};
}