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

	struct AssetDescriptor {
		uint32 identifier;
		VersionNumber version;
	};

	class Asset {
		public:
			explicit Asset(const AssetDescriptor& descriptor) :
				identifier(descriptor.identifier),
				version(descriptor.version)
			{}

			inline uint32 getIdentifier() const {
				return identifier;
			}

			inline VersionNumber getVersion() const {
				return version;
			}

		private:
			uint32 identifier;
			VersionNumber version;
	};
}