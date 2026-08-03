// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/Asset.hpp"

namespace Trident {
	struct MaterialAsset : public Asset {
		// TODO(asset/material): Keep this here?
		uint32 shaderModuleAssetIdentifier;

		sequence<uint32> descriptorImageAssetIdentifiers;
	};
}