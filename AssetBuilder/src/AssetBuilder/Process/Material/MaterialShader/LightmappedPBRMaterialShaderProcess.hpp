// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/MaterialShaderProcess.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident {
	struct LightmappedPBRMaterialDescriptor {
		view<char8> armTexturePath;
		view<char8> diffuseTexturePath;

		/// Must be specified in GL format.
		view<char8> normalTexturePath;
	};

	class LightmappedPBRMaterialShaderProcess : public MaterialShaderProcess {
		protected:
			explicit LightmappedPBRMaterialShaderProcess(AssetBuilderLibrary* assetLibrary, const LightmappedPBRMaterialDescriptor& descriptor) :
				MaterialShaderProcess(assetLibrary),
				descriptor(descriptor)
			{}

			MaterialAsset createMaterialAsset(std::filesystem::path rootOutputPath) const final override {
				AssetLibrary assetLibrary(rootOutputPath);

				MaterialAsset asset{};

				// TODO(tsl): Use a real identifier when TSL is ready.
				asset.shaderModuleAssetIdentifier = 401;

				asset.descriptorImageAssetIdentifiers = {
					assetLibrary.getAsset<ImageAsset>(descriptor.armTexturePath)->identifier,
					assetLibrary.getAsset<ImageAsset>(descriptor.diffuseTexturePath)->identifier,
					assetLibrary.getAsset<ImageAsset>(descriptor.normalTexturePath)->identifier,
				};

				return asset;
			}

		private:
			LightmappedPBRMaterialDescriptor descriptor;
	};
}