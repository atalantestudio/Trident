#pragma once

namespace ProjectA {
	class VersionNumber {
		public:
			explicit VersionNumber(uint8 major, uint8 minor, uint16 patch) :
				major(major),
				minor(minor),
				patch(patch)
			{}

		public:
			uint8 major;
			uint8 minor;
			uint16 patch;
	};
}