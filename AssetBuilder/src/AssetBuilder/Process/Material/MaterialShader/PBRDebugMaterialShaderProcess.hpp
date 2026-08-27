// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/PBRMaterialShaderProcess.hpp"

namespace Trident {
	class PBRDebugMaterialShaderProcess : public MaterialShaderProcess {
		protected:
			explicit PBRDebugMaterialShaderProcess(AssetBuilderLibrary* assetLibrary, const PBRMaterialDescriptor& descriptor) :
				MaterialShaderProcess(assetLibrary),
				descriptor(descriptor)
			{}

			MaterialAsset createMaterialAsset(std::filesystem::path rootOutputPath) const final override {
				AssetLibrary assetLibrary(rootOutputPath);

				MaterialAsset asset{};

				// TODO(tsl): Use a real identifier when TSL is ready.
				asset.shaderModuleAssetIdentifier = 403;

				asset.descriptorImageAssetIdentifiers = {
					assetLibrary.getAsset<ImageAsset>(descriptor.armTexturePath)->identifier,
					assetLibrary.getAsset<ImageAsset>(descriptor.diffuseTexturePath)->identifier,
					assetLibrary.getAsset<ImageAsset>(descriptor.normalTexturePath)->identifier,
				};

				return asset;
			}

		private:
			PBRMaterialDescriptor descriptor;
	};
}