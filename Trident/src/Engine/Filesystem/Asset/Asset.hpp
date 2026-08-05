// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Base/VersionNumber.hpp"
#include "Engine/Filesystem/Asset/AssetSignature.hpp"

namespace Trident {
	struct Asset {
		uint32 identifier;
		VersionNumber version;
	};

	struct AssetHeader {
		AssetSignature signature;
		uint32 identifier;
		VersionNumber version;
	};
}