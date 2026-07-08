// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class AssetMap {
		public:
			inline view<char8> operator[](uint32 identifier) const {
				const auto asset = map.find(identifier);

				DEBUG_ASSERT(asset != map.end());

				return asset->second;
			}

		public:
			sequence<uint8> data;

			/// Not including the file extension.
			std::unordered_map<uint32, view<char8>> map;
	};
}