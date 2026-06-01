// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct AssetChunk {
		AssetSignature signature;

		/// Not including the file extension.
		view<char8> assetPath;
	};

	struct NewAssetChunk {
		AssetSignature signature;

		/// Not including the file extension.
		sequence<char8> assetPath;
	};

	class AssetMap {
		public:
			inline AssetChunk operator[](uint32 identifier) const {
				auto chunk = map.find(identifier);

				DEBUG_ASSERT(chunk != map.end());

				return chunk->second;
			}

			inline void add(uint32 identifier, AssetSignature signature, view<char8> path) {
				newAssets.insert({identifier, {signature, path}});
			}

		private:
			sequence<char8> data;
			std::unordered_map<uint32, AssetChunk> map;

		public:
			std::unordered_map<uint32, NewAssetChunk> newAssets;
	};
}