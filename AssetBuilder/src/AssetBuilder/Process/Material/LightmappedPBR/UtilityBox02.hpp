// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/LightmappedPBRMaterialShaderProcess.hpp"

namespace Trident {
	class UtilityBox02Material final : public LightmappedPBRMaterialShaderProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "lpbr_utility_box_02_1k";
			}

		public:
			explicit UtilityBox02Material(AssetBuilderLibrary* assetLibrary) :
				LightmappedPBRMaterialShaderProcess(assetLibrary, {
					.armMapPath = "utility_box_02_arm_1k",
					.diffuseMapPath = "utility_box_02_diff_1k",
					.normalMapPath = "utility_box_02_nor_gl_1k",
				})
			{}
	};
}