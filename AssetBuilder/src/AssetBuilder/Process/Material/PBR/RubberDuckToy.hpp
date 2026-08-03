// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/PBRMaterialShaderProcess.hpp"

namespace Trident {
	class RubberDuckToyMaterial final : public PBRMaterialShaderProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "rubber_duck_toy_1k";
			}

		public:
			explicit RubberDuckToyMaterial(AssetBuilderLibrary* assetLibrary) :
				PBRMaterialShaderProcess(assetLibrary, {
					.armTexturePath = "rubber_duck_toy_arm_1k",
					.diffuseTexturePath = "rubber_duck_toy_diff_1k",
					.normalTexturePath = "rubber_duck_toy_nor_gl_1k",
				})
			{}
	};
}