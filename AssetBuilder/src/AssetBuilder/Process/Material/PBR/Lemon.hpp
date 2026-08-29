// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/PBRMaterialShaderProcess.hpp"

namespace Trident {
	class LemonMaterial final : public PBRMaterialShaderProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "lemon_1k";
			}

		public:
			explicit LemonMaterial(AssetBuilderLibrary* assetLibrary) :
				PBRMaterialShaderProcess(assetLibrary, {
					.armMapPath = "lemon_arm_1k",
					.diffuseMapPath = "lemon_diff_1k",
					.normalMapPath = "lemon_nor_gl_1k",
				})
			{}
	};
}