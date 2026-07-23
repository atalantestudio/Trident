// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/LightmappedMaterialShaderProcess.hpp"

namespace Trident {
	class PlasterCeilingMaterialProcess final : public LightmappedMaterialShaderProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "plasterceiling";
			}

		public:
			explicit PlasterCeilingMaterialProcess(AssetBuilderLibrary* assetLibrary) :
				LightmappedMaterialShaderProcess(assetLibrary, {
					.diffuseTexturePath = "plasterwall030a",
				})
			{}
	};
}