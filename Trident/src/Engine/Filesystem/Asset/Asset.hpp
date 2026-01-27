#pragma once

#include "Engine/Filesystem/Asset/AssetSignature.hpp"

namespace Trident {
	struct AssetDescriptor {
		uint32 identifier;
		VersionNumber version;
	};

	class Asset {
		public:
			static constexpr uint64 HEADER_SIZE = sizeof(AssetSignature) + sizeof(uint32) + sizeof(VersionNumber);

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