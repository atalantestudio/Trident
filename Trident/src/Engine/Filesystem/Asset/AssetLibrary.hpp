// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/AssetMap.hpp"
#include "Engine/Filesystem/Asset/FontAsset.hpp"
#include "Engine/Filesystem/Asset/Geometry2DAsset.hpp"
#include "Engine/Filesystem/Asset/Geometry3DAsset.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Filesystem/Asset/MaterialAsset.hpp"
#include "Engine/Filesystem/Asset/Scene2DAsset.hpp"
#include "Engine/Filesystem/Asset/Scene3DAsset.hpp"
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

		private:
			static constexpr uint64 ASSET_COUNT_PER_TYPE = 16;

		public:
			explicit AssetLibrary(std::filesystem::path basePath);

			const std::unordered_map<uint32, view<char8>>& getAssetMap() const;

			void loadAssetMap();

			template<typename AssetType>
			ref<AssetType> getAsset(uint32 identifier) {
				const auto path = assetMap.map.find(identifier);

				ENGINE_ASSERT(path != assetMap.map.end(), "Asset [] is not registered in the asset map.", identifier);

				return getAsset<AssetType>(path->second);
			}

			template<typename AssetType>
			ref<AssetType> getAsset(view<char8> path) {
				const std::unordered_map<sequence<char8>, uint32>& assetPathToIndexMap = getAssetPathToIndexMap<AssetType>();

				auto index = assetPathToIndexMap.find(path);

				if (index == assetPathToIndexMap.end()) {
					loadCachedAsset<AssetType>(path);

					index = assetPathToIndexMap.find(path);
				}

				return getLoadedAssets<AssetType>()[index->second];
			}

			template<typename AssetType>
			void loadCachedAsset(uint32 identifier) {
				const auto path = assetMap.map.find(identifier);

				ENGINE_ASSERT(path != assetMap.map.end(), "Asset [] is not registered in the asset map.", identifier);

				const std::unordered_map<sequence<char8>, uint32>& assetPathToIndexMap = getAssetPathToIndexMap<AssetType>();

				auto index = assetPathToIndexMap.find(path->second);

				if (index == assetPathToIndexMap.end()) {
					loadCachedAsset<AssetType>(path->second);
				}
			}

			uint32 getLoadedAssetIndex(uint32 identifier) const;

			template<typename AssetType>
			view<AssetType> getLoadedAssets() const;

		private:
			std::filesystem::path getAbsolutePath(view<char8> base, view<char8> relativePath, view<char8> extension) const;

			template<typename AssetType>
			const std::unordered_map<sequence<char8>, uint32>& getAssetPathToIndexMap() const;

			template<typename AssetType>
			void loadCachedAsset(view<char8> path);

		protected:
			AssetMap assetMap;

		private:
			std::filesystem::path basePath;
			sequence<FontAsset> fontAssets;
			sequence<Geometry2DAsset> geometry2DAssets;
			sequence<Geometry3DAsset> geometry3DAssets;
			sequence<ImageAsset> imageAssets;
			sequence<MaterialAsset> materialAssets;
			sequence<Scene2DAsset> scene2DAssets;
			sequence<Scene3DAsset> scene3DAssets;
			sequence<ShaderModuleAsset> shaderModuleAssets;
			uint64 fontAssetCount = 0;
			uint64 geometry2DAssetCount = 0;
			uint64 geometry3DAssetCount = 0;
			uint64 imageAssetCount = 0;
			uint64 materialAssetCount = 0;
			uint64 scene2DAssetCount = 0;
			uint64 scene3DAssetCount = 0;
			uint64 shaderModuleAssetCount = 0;
			std::unordered_map<uint32, uint32> assetIdentifierToIndex;
			std::unordered_map<sequence<char8>, uint32> fontAssetPathToIndex;
			std::unordered_map<sequence<char8>, uint32> geometryAssetPathToIndex;
			std::unordered_map<sequence<char8>, uint32> imageAssetPathToIndex;
			std::unordered_map<sequence<char8>, uint32> materialAssetPathToIndex;
			std::unordered_map<sequence<char8>, uint32> sceneAssetPathToIndex;
			std::unordered_map<sequence<char8>, uint32> shaderModuleAssetPathToIndex;
	};
}