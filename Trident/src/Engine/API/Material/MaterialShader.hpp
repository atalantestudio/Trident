// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

namespace Trident {
	struct MaterialShader {
		ref<ShaderModuleAsset> shaderModuleAsset;
		range<uint32, uint32> materialAssetRange;
	};
}