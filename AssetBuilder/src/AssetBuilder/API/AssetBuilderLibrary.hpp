// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/BuildAssetMap.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"

namespace Trident {
	/// An extension of the runtime asset library which tracks the identifiers of previously loaded assets.
	class AssetBuilderLibrary : public AssetLibrary {
		public:
			explicit AssetBuilderLibrary(std::filesystem::path basePath) :
				AssetLibrary(basePath)
			{}

			void registerAsset(uint32 identifier, const sequence<char8>& path) {
				buildAssetMap.map.insert({identifier, path});

				const view<char8> pathView = buildAssetMap.map.find(identifier)->second;

				assetMap.map.insert({identifier, pathView});
			}

		public:
			BuildAssetMap buildAssetMap;
	};
}