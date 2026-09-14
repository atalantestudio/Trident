// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/LightmappedPBRMaterialShaderProcess.hpp"

namespace Trident {
	class UtilityBox01Material final : public LightmappedPBRMaterialShaderProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "lpbr_utility_box_01_1k";
			}

		public:
			explicit UtilityBox01Material(AssetBuilderLibrary* assetLibrary) :
				LightmappedPBRMaterialShaderProcess(assetLibrary, {
					.armMapPath = "utility_box_01_arm_1k",
					.diffuseMapPath = "utility_box_01_diff_1k",
					.normalMapPath = "utility_box_01_nor_gl_1k",
				})
			{}
	};
}