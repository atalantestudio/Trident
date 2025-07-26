#pragma once

namespace ProjectA {
	class Asset {
		public:
			explicit Asset() :
				id(random<uint32>(min<uint32>(), max<uint32>()))
			{}

			uint32 getId() const {
				return id;
			}

		private:
			uint32 id;
	};
}