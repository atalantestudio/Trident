#pragma once

namespace ProjectA {
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

		public:
			uint32 identifier;
			VersionNumber version;
	};
}