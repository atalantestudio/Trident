// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/AssetMap.hpp"
#include "Engine/Filesystem/Asset/FontAsset.hpp"
#include "Engine/Filesystem/Asset/GeometryAsset.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Filesystem/Asset/MaterialAsset.hpp"
#include "Engine/Filesystem/Asset/SceneAsset.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace Trident {
	class AssetLibrary {
		public:
			/// Creates and returns a unique asset identifier.
			/// See https://en.wikipedia.org/wiki/Ostrich_algorithm.
			inline static uint32 createAssetIdentifier() {
				return random<uint32>(min<uint32>(), max<uint32>());
			}

		public:
			explicit AssetLibrary(std::filesystem::path basePath);

			const std::unordered_map<uint32, view<char8>>& getAssetMap() const;

			void loadAssetMap();

			template<typename AssetType>
			ref<AssetType> getAsset(uint32 identifier) {
				const auto path = assetMap.map.find(identifier);

				ENGINE_ASSERT(path != assetMap.map.end(), "Asset [] is not registered in the asset map.", identifier);

				const std::unordered_map<sequence<char8>, uint32>& assetPathToIndexMap = getAssetPathToIndexMap<AssetType>();

				auto index = assetPathToIndexMap.find(path->second);

				if (index == assetPathToIndexMap.end()) {
					loadAsset<AssetType>(path->second);

					index = assetPathToIndexMap.find(path->second);
				}

				return std::static_pointer_cast<AssetType>(assets[index->second]);
			}

			template<typename AssetType>
			ref<AssetType> getAsset(view<char8> path) {
				const std::unordered_map<sequence<char8>, uint32>& assetPathToIndexMap = getAssetPathToIndexMap<AssetType>();

				auto index = assetPathToIndexMap.find(path);

				if (index == assetPathToIndexMap.end()) {
					loadAsset<AssetType>(path);

					index = assetPathToIndexMap.find(path);
				}

				return std::static_pointer_cast<AssetType>(assets[index->second]);
			}

		private:
			std::filesystem::path getAbsolutePath(view<char8> base, view<char8> relativePath, view<char8> extension) const;

		private:
			template<typename AssetType>
			const std::unordered_map<sequence<char8>, uint32>& getAssetPathToIndexMap() const;

			template<typename AssetType>
			void loadAsset(view<char8> path);

		private:
			std::filesystem::path basePath;
			AssetMap assetMap;
			std::vector<ref<Asset>> assets;
			std::unordered_map<uint32, uint32> assetIdentifierToIndex;
			std::unordered_map<sequence<char8>, uint32> fontAssetPathToIndex;
			std::unordered_map<sequence<char8>, uint32> geometryAssetPathToIndex;
			std::unordered_map<sequence<char8>, uint32> imageAssetPathToIndex;
			std::unordered_map<sequence<char8>, uint32> materialAssetPathToIndex;
			std::unordered_map<sequence<char8>, uint32> sceneAssetPathToIndex;
			std::unordered_map<sequence<char8>, uint32> shaderModuleAssetPathToIndex;
	};

	ref<FontAsset> decodeFontAsset(ByteStream<StreamOperation::READ>& stream, AssetLibrary* assetLibrary, view<char8> path);
}