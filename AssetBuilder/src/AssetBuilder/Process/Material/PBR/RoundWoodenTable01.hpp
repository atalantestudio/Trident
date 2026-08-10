// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/PBRMaterialShaderProcess.hpp"

namespace Trident {
	class RoundWoodenTable01Material final : public PBRMaterialShaderProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "round_wooden_table_01_1k";
			}

		public:
			explicit RoundWoodenTable01Material(AssetBuilderLibrary* assetLibrary) :
				PBRMaterialShaderProcess(assetLibrary, {
					.armTexturePath = "round_wooden_table_01_arm_1k",
					.diffuseTexturePath = "round_wooden_table_01_diff_1k",
					.normalTexturePath = "round_wooden_table_01_nor_gl_1k",
				})
			{}
	};
}