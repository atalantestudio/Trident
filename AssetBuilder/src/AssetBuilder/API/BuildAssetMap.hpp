// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class BuildAssetMap {
		public:
			void insert(uint32 identifier, sequence<char8> path) {
				for (const auto& asset : map) {
					if (path == asset.second) {
						map[asset.first] = path;

						return;
					}
				}

				map.insert({identifier, path});
			}

		public:
			std::unordered_map<uint32, sequence<char8>> map;
	};
}