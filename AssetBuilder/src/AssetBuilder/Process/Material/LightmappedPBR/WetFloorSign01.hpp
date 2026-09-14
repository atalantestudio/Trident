// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/LightmappedPBRMaterialShaderProcess.hpp"

namespace Trident {
	class WetFloorSign01Material final : public LightmappedPBRMaterialShaderProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "lpbr_wet_floor_sign_01_1k";
			}

		public:
			explicit WetFloorSign01Material(AssetBuilderLibrary* assetLibrary) :
				LightmappedPBRMaterialShaderProcess(assetLibrary, {
					.armMapPath = "wet_floor_sign_01_arm_1k",
					.diffuseMapPath = "wet_floor_sign_01_diff_1k",
					.normalMapPath = "wet_floor_sign_01_nor_gl_1k",
				})
			{}
	};
}