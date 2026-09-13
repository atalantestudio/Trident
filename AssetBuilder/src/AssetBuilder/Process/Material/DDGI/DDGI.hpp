// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/Process/Material/MaterialShader/DDGIMaterialShaderProcess.hpp"

namespace Trident {
	class DDGIMaterial final : public DDGIMaterialShaderProcess {
		protected:
			view<char8> getAssetName() const final override {
				return "ddgi";
			}

		public:
			explicit DDGIMaterial(AssetBuilderLibrary* assetLibrary) :
				DDGIMaterialShaderProcess(assetLibrary, {
					.diffuseMapPath = "white",
					.normalMapPath = "normal",
				})
			{}
	};
}