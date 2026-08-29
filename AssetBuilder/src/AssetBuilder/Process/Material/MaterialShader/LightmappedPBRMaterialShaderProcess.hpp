// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/MaterialShaderProcess.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident {
	struct LightmappedPBRMaterialDescriptor {
		view<char8> armMapPath;
		view<char8> diffuseMapPath;

		/// Must be specified in GL format.
		view<char8> normalMapPath;
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
					assetLibrary.getAsset<ImageAsset>(descriptor.armMapPath)->identifier,
					assetLibrary.getAsset<ImageAsset>(descriptor.diffuseMapPath)->identifier,
					assetLibrary.getAsset<ImageAsset>(descriptor.normalMapPath)->identifier,
				};

				return asset;
			}

		private:
			LightmappedPBRMaterialDescriptor descriptor;
	};
}