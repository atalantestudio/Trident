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

				// TODO(tsl): Use a real identifier when TSL is ready.
				asset.shaderModuleAssetIdentifier = 400;

				asset.descriptorImageAssetIdentifiers = {
					assetLibrary.getAsset<ImageAsset>(descriptor.diffuseTexturePath)->identifier,
				};

				return asset;
			}

		private:
			LightmappedMaterialDescriptor descriptor;
	};
}