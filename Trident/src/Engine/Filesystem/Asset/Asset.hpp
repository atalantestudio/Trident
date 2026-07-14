// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/AssetSignature.hpp"

namespace Trident {
	struct AssetHeader {
		AssetSignature signature;
		uint32 identifier;
		VersionNumber version;
	};

	struct Asset {
		uint32 identifier;
		VersionNumber version;
	};
}