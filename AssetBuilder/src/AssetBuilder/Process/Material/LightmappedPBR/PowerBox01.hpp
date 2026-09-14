// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/LightmappedPBRMaterialShaderProcess.hpp"

namespace Trident {
	class PowerBox01Material final : public LightmappedPBRMaterialShaderProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "lpbr_power_box_01_1k";
			}

		public:
			explicit PowerBox01Material(AssetBuilderLibrary* assetLibrary) :
				LightmappedPBRMaterialShaderProcess(assetLibrary, {
					.armMapPath = "power_box_01_arm_1k",
					.diffuseMapPath = "power_box_01_diff_1k",
					.normalMapPath = "power_box_01_nor_gl_1k",
				})
			{}
	};
}