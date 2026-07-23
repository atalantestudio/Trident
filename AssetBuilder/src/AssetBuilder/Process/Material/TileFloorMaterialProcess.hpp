// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/LightmappedMaterialShaderProcess.hpp"

namespace Trident {
	class TileFloorMaterialProcess final : public LightmappedMaterialShaderProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "tilefloor";
			}

		public:
			explicit TileFloorMaterialProcess(AssetBuilderLibrary* assetLibrary) :
				LightmappedMaterialShaderProcess(assetLibrary, {
					.diffuseTexturePath = "tilefloor018a",
				})
			{}
	};
}