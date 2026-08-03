// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/LightmappedPBRMaterialShaderProcess.hpp"

namespace Trident {
	class LPBRGallineraTableMaterial final : public LightmappedPBRMaterialShaderProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "lpbr_gallinera_table_1k";
			}

		public:
			explicit LPBRGallineraTableMaterial(AssetBuilderLibrary* assetLibrary) :
				LightmappedPBRMaterialShaderProcess(assetLibrary, {
					.armTexturePath = "gallinera_table_arm_1k",
					.diffuseTexturePath = "gallinera_table_diff_1k",
					.normalTexturePath = "gallinera_table_nor_gl_1k",
				})
			{}
	};
}