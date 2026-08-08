// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/LightmappedMaterialShaderProcess.hpp"

namespace Trident {
	class LMDefaultMaterial final : public LightmappedMaterialShaderProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "lm_default";
			}

		public:
			explicit LMDefaultMaterial(AssetBuilderLibrary* assetLibrary) :
				LightmappedMaterialShaderProcess(assetLibrary, {
					.diffuseTexturePath = "white",
				})
			{}
	};
}