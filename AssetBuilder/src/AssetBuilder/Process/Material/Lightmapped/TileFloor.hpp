// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/LightmappedMaterialShaderProcess.hpp"

namespace Trident {
	class LMTileFloorMaterial final : public LightmappedMaterialShaderProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "lm_tile_floor";
			}

		public:
			explicit LMTileFloorMaterial(AssetBuilderLibrary* assetLibrary) :
				LightmappedMaterialShaderProcess(assetLibrary, {
					.diffuseMapPath = "tilefloor018a",
					.normalMapPath = "tilefloor018a_normal",
				})
			{}
	};
}