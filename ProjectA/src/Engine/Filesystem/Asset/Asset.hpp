#pragma once

namespace ProjectA {
	struct AssetDescriptor {
		uint32 identifier;
		VersionNumber version;
	};

	class Asset {
		public:
			explicit Asset() :
				identifier(random<uint32>(min<uint32>(), max<uint32>())),
				version(0, 0, 1)
			{}

			explicit Asset(const AssetDescriptor& descriptor) :
				identifier(descriptor.identifier),
				version(descriptor.version)
			{}

		public:
			uint32 identifier;
			VersionNumber version;
	};
}