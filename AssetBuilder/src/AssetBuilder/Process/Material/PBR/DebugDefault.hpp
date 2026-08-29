// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/PBRDebugMaterialShaderProcess.hpp"

namespace Trident {
	class PBRDebugDefaultMaterial final : public PBRDebugMaterialShaderProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "pbr_debug_default";
			}

		public:
			explicit PBRDebugDefaultMaterial(AssetBuilderLibrary* assetLibrary) :
				PBRDebugMaterialShaderProcess(assetLibrary, {
					.armMapPath = "white",
					.diffuseMapPath = "red",
					.normalMapPath = "normal",
				})
			{}
	};
}