// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/AssetSignature.hpp"

namespace Trident {
	struct AssetDescriptor {
		uint32 identifier;
		VersionNumber version;
	};

	struct AssetHeader {
		AssetSignature signature;
		uint32 identifier;
		VersionNumber version;
	};

	class Asset {
		public:
			explicit Asset(const AssetDescriptor& descriptor) :
				identifier(descriptor.identifier),
				version(descriptor.version)
			{}

		public:
			uint32 identifier;
			VersionNumber version;
	};
}