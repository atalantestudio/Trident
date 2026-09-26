// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/LightmappedMaterialShaderProcess.hpp"

namespace Trident {
	class LMDebugWallMaterial final : public LightmappedMaterialShaderProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "lm_debug_wall";
			}

		public:
			explicit LMDebugWallMaterial(AssetBuilderLibrary* assetLibrary) :
				LightmappedMaterialShaderProcess(assetLibrary, {
					.diffuseMapPath = "dev_measurewall01a",
					.normalMapPath = "normal",
				})
			{}
	};
}