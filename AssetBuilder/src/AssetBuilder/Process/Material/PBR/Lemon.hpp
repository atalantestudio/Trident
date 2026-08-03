// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/PBRSeparatedAORoughnessMaterialShaderProcess.hpp"

namespace Trident {
	class LemonMaterial final : public PBRSeparatedAORoughnessMaterialShaderProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "lemon_1k";
			}

		public:
			explicit LemonMaterial(AssetBuilderLibrary* assetLibrary) :
				PBRSeparatedAORoughnessMaterialShaderProcess(assetLibrary, {
					.aoTexturePath = "lemon_ao_1k",
					.roughnessTexturePath = "lemon_rough_1k",
					.diffuseTexturePath = "lemon_diff_1k",
					.normalTexturePath = "lemon_nor_gl_1k",
				})
			{}
	};
}