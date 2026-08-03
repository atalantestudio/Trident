// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/PBRMaterialShaderProcess.hpp"

namespace Trident {
	class FoldingWoodenStoolMaterial final : public PBRMaterialShaderProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "folding_wooden_stool_1k";
			}

		public:
			explicit FoldingWoodenStoolMaterial(AssetBuilderLibrary* assetLibrary) :
				PBRMaterialShaderProcess(assetLibrary, {
					.armTexturePath = "folding_wooden_stool_arm_1k",
					.diffuseTexturePath = "folding_wooden_stool_diff_1k",
					.normalTexturePath = "folding_wooden_stool_nor_gl_1k",
				})
			{}
	};
}