// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/FontAsset.hpp"
#include "Engine/Filesystem/Asset/GeometryAsset.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"
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

		private:
			static std::filesystem::path getAbsolutePath(view<char8> base, view<char8> relativePath, view<char8> extension);

		public:
			template<typename AssetType>
			ref<AssetType> getAsset(uint32 identifier) const {
				const auto iterator = assetIdentifierToIndex.find(identifier);

				ENGINE_ASSERT(iterator != assetIdentifierToIndex.end(), "Asset is not loaded.");

				return std::static_pointer_cast<AssetType>(assets[iterator->second]);
			}

			template<typename AssetType>
			ref<AssetType> getAsset(uint32 identifier, view<char8> path) {
				auto iterator = assetIdentifierToIndex.find(identifier);

				if (iterator == assetIdentifierToIndex.end()) {
					loadAsset<AssetType>(path);

					iterator = assetIdentifierToIndex.find(identifier);
				}

				return std::static_pointer_cast<AssetType>(assets[iterator->second]);
			}

			template<typename AssetType>
			ref<AssetType> getAsset(view<char8> path) {
				const std::unordered_map<sequence<char8>, uint32>& assetPathToIndexMap = getAssetPathToIndexMap<AssetType>();

				auto iterator = assetPathToIndexMap.find(path);

				if (iterator == assetPathToIndexMap.end()) {
					loadAsset<AssetType>(path);

					iterator = assetPathToIndexMap.find(path);
				}

				return std::static_pointer_cast<AssetType>(assets[iterator->second]);
			}

		private:
			template<typename AssetType>
			const std::unordered_map<sequence<char8>, uint32>& getAssetPathToIndexMap() const;

			template<typename AssetType>
			void loadAsset(view<char8> path);

			ref<FontAsset> decodeFontAsset(view<char8> path, ByteStream<StreamOperation::READ>& byteStream);
			ref<ImageAsset> decodeImageAsset(ByteStream<StreamOperation::READ>& byteStream) const;
			ref<Scene2DAsset> decodeScene2DAsset(ByteStream<StreamOperation::READ>& stream) const;
			ref<Scene3DAsset> decodeScene3DAsset(ByteStream<StreamOperation::READ>& stream) const;
			ref<ShaderModuleAsset> decodeShaderModuleAsset(ByteStream<StreamOperation::READ>& byteStream) const;

		private:
			std::vector<ref<Asset>> assets;
			std::unordered_map<uint32, uint32> assetIdentifierToIndex;
			std::unordered_map<sequence<char8>, uint32> fontAssetPathToIndex;
			std::unordered_map<sequence<char8>, uint32> geometryAssetPathToIndex;
			std::unordered_map<sequence<char8>, uint32> imageAssetPathToIndex;
			std::unordered_map<sequence<char8>, uint32> sceneAssetPathToIndex;
			std::unordered_map<sequence<char8>, uint32> shaderModuleAssetPathToIndex;
	};
}