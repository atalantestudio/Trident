// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/PBRMaterialShaderProcess.hpp"

namespace Trident {
	class PBRBlueMetalPlateMaterial final : public PBRMaterialShaderProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "pbr_blue_metal_plate_1k";
			}

		public:
			explicit PBRBlueMetalPlateMaterial(AssetBuilderLibrary* assetLibrary) :
				PBRMaterialShaderProcess(assetLibrary, {
					.armTexturePath = "blue_metal_plate_arm_1k",
					.diffuseTexturePath = "blue_metal_plate_diff_1k",
					.normalTexturePath = "blue_metal_plate_nor_gl_1k",
				})
			{}
	};
}