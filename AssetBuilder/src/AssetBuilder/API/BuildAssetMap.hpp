// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class BuildAssetMap {
		public:
			void insert(uint32 identifier, sequence<char8> path) {
				// TODO(asset-builder/duplicate-identifier)
				// Some assets have the same name, for example MyScene.tsa (scene) and MyScene.tia (lightmap atlas).
				// We must store both but this will also leave old unused assets unaffected.
				/* for (const auto& asset : map) {
					if (path == asset.second) {
						map[asset.first] = path;

						return;
					}
				} */

				map.insert({identifier, path});
			}

		public:
			std::unordered_map<uint32, sequence<char8>> map;
	};
}