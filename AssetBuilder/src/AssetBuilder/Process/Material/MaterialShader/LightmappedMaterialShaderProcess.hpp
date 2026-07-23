// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/MaterialShaderProcess.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident {
	struct LightmappedMaterialDescriptor {
		view<char8> diffuseTexturePath;
	};

	class LightmappedMaterialShaderProcess : public MaterialShaderProcess {
		protected:
			explicit LightmappedMaterialShaderProcess(AssetBuilderLibrary* assetLibrary, const LightmappedMaterialDescriptor& descriptor) :
				MaterialShaderProcess(assetLibrary),
				descriptor(descriptor)
			{}

			MaterialAsset createMaterialAsset(std::filesystem::path rootOutputPath) const final override {
				AssetLibrary assetLibrary(rootOutputPath);

				MaterialAsset asset{};

				// TODO(asset-builder/material): Find another way of specifying the shader index that's not relative to the current scene.
				asset.shaderIndex = 0;

				asset.descriptorOffset = 1;
				asset.descriptorImages = {
					{
						.imageAsset = assetLibrary.getAsset<ImageAsset>(descriptor.diffuseTexturePath),
					},
				};

				return asset;
			}

		private:
			LightmappedMaterialDescriptor descriptor;
	};
}