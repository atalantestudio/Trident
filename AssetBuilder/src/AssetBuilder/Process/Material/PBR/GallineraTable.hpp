// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/PBRMaterialShaderProcess.hpp"

namespace Trident {
	class GallineraTableMaterial final : public PBRMaterialShaderProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "gallinera_table_1k";
			}

		public:
			explicit GallineraTableMaterial(AssetBuilderLibrary* assetLibrary) :
				PBRMaterialShaderProcess(assetLibrary, {
					.armMapPath = "gallinera_table_arm_1k",
					.diffuseMapPath = "gallinera_table_diff_1k",
					.normalMapPath = "gallinera_table_nor_gl_1k",
				})
			{}
	};
}