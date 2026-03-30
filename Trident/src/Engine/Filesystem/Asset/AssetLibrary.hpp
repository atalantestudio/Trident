// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/AssetSignature.hpp"
#include "Engine/Filesystem/Asset/FontAsset.hpp"
#include "Engine/Filesystem/Asset/GeometryAsset.hpp"
#include "Engine/Filesystem/Asset/ImageAsset.hpp"
#include "Engine/Filesystem/Asset/SceneAsset.hpp"
#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace Trident {
	class AssetLibrary {
		public:
			/// Creates and returns a unique asset identifier.
			/// See https://en.wikipedia.org/wiki/Ostrich_algorithm.
			static uint32 createAssetIdentifier() {
				return random<uint32>(min<uint32>(), max<uint32>());
			}

		private:
			static std::filesystem::path getAbsolutePath(view<char8> base, view<char8> relativePath, view<char8> extension) {
				sequence<char8> path(relativePath.count() + extension.count());

				std::copy(relativePath.begin(), relativePath.end(), &path[0]);
				std::copy(extension.begin(), extension.end(), &path[relativePath.count()]);

				return std::filesystem::path(base.begin(), base.end()) / std::filesystem::path(path.begin(), path.end());
			}

		public:
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

			template<>
			const std::unordered_map<sequence<char8>, uint32>& getAssetPathToIndexMap<FontAsset>() const {
				return fontAssetPathToIndex;
			}

			template<>
			const std::unordered_map<sequence<char8>, uint32>& getAssetPathToIndexMap<GeometryAsset>() const {
				return geometryAssetPathToIndex;
			}

			template<>
			const std::unordered_map<sequence<char8>, uint32>& getAssetPathToIndexMap<ImageAsset>() const {
				return imageAssetPathToIndex;
			}

			template<>
			const std::unordered_map<sequence<char8>, uint32>& getAssetPathToIndexMap<Scene2DAsset>() const {
				return sceneAssetPathToIndex;
			}

			template<>
			const std::unordered_map<sequence<char8>, uint32>& getAssetPathToIndexMap<ShaderModuleAsset>() const {
				return shaderModuleAssetPathToIndex;
			}

			template<typename AssetType>
			void loadAsset(view<char8> path);

			template<>
			void loadAsset<FontAsset>(view<char8> path) {
				const std::filesystem::path absolutePath = getAbsolutePath("Asset/Font", path, ".tfo");
				const sequence<char8> assetPath = absolutePath.string();
				const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

				ByteStream<StreamOperation::READ> byteStream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

				if (byteStream.getRemainingSize() < sizeof(AssetSignature) || byteStream.peek<AssetSignature>() != AssetSignature::FONT) {
					return;
				}

				byteStream.jump(sizeof(AssetSignature));

				const ref<FontAsset> fontAsset = decodeFontAsset(path, byteStream);
				const uint32 fontAssetIndex = static_cast<uint32>(assets.size());
				const sequence<char8> stem = absolutePath.stem().string();

				assetIdentifierToIndex.insert({fontAsset->identifier, fontAssetIndex});
				fontAssetPathToIndex.insert({stem, fontAssetIndex});
				assets.emplace_back(fontAsset);
			}

			template<>
			void loadAsset<GeometryAsset>(view<char8> path) {
				const std::filesystem::path absolutePath = getAbsolutePath("Asset/Geometry", path, ".tge");
				const sequence<char8> assetPath = absolutePath.string();
				const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

				ByteStream<StreamOperation::READ> byteStream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

				if (byteStream.getRemainingSize() < sizeof(AssetSignature) || byteStream.peek<AssetSignature>() != AssetSignature::GEOMETRY) {
					return;
				}

				byteStream.jump(sizeof(AssetSignature));

				const ref<GeometryAsset> geometryAsset = Filesystem::decodeAsset<GeometryAsset>(byteStream);
				const uint32 geometryAssetIndex = static_cast<uint32>(assets.size());
				const sequence<char8> stem = absolutePath.stem().string();

				assetIdentifierToIndex.insert({geometryAsset->identifier, geometryAssetIndex});
				geometryAssetPathToIndex.insert({stem, geometryAssetIndex});
				assets.emplace_back(geometryAsset);
			}

			template<>
			void loadAsset<ImageAsset>(view<char8> path) {
				const std::filesystem::path absolutePath = getAbsolutePath("Asset/Image", path, ".tia");
				const sequence<char8> assetPath = absolutePath.string();
				const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

				ByteStream<StreamOperation::READ> byteStream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

				if (byteStream.getRemainingSize() < sizeof(AssetSignature) || byteStream.peek<AssetSignature>() != AssetSignature::IMAGE) {
					return;
				}

				const ref<ImageAsset> imageAsset = decodeImageAsset(byteStream);
				const uint32 imageAssetIndex = static_cast<uint32>(assets.size());
				const sequence<char8> stem = absolutePath.stem().string();

				assetIdentifierToIndex.insert({imageAsset->identifier, imageAssetIndex});
				imageAssetPathToIndex.insert({stem, imageAssetIndex});
				assets.emplace_back(imageAsset);
			}

			template<>
			void loadAsset<Scene2DAsset>(view<char8> path) {
				const std::filesystem::path absolutePath = getAbsolutePath("Asset/Scene", path, ".tsa");
				const sequence<char8> assetPath = absolutePath.string();
				const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

				ByteStream<StreamOperation::READ> byteStream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

				if (byteStream.getRemainingSize() < sizeof(AssetSignature) || byteStream.peek<AssetSignature>() != AssetSignature::SCENE) {
					return;
				}

				byteStream.jump(sizeof(AssetSignature));

				const ref<Scene2DAsset> sceneAsset = decodeScene2DAsset(byteStream);
				const uint32 sceneAssetIndex = static_cast<uint32>(assets.size());
				const sequence<char8> stem = absolutePath.stem().string();

				assetIdentifierToIndex.insert({sceneAsset->identifier, sceneAssetIndex});
				sceneAssetPathToIndex.insert({stem, sceneAssetIndex});
				assets.emplace_back(sceneAsset);
			}

			template<>
			void loadAsset<ShaderModuleAsset>(view<char8> path) {
				const std::filesystem::path absolutePath = getAbsolutePath("Asset/Shader", path, ".tsm");
				const sequence<char8> assetPath = absolutePath.string();
				const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

				ByteStream<StreamOperation::READ> byteStream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

				if (byteStream.getRemainingSize() < sizeof(AssetSignature) || byteStream.peek<AssetSignature>() != AssetSignature::SHADER_MODULE) {
					return;
				}

				byteStream.jump(sizeof(AssetSignature));

				const ref<ShaderModuleAsset> shaderModuleAsset = decodeShaderModuleAsset(byteStream);
				const uint32 shaderModuleAssetIndex = static_cast<uint32>(assets.size());
				const sequence<char8> stem = absolutePath.stem().string();

				assetIdentifierToIndex.insert({shaderModuleAsset->identifier, shaderModuleAssetIndex});
				shaderModuleAssetPathToIndex.insert({stem, shaderModuleAssetIndex});
				assets.emplace_back(shaderModuleAsset);
			}

			ref<FontAsset> decodeFontAsset(view<char8> path, ByteStream<StreamOperation::READ>& byteStream) {
				FontAssetDescriptor fontAssetDescriptor{};
				fontAssetDescriptor.identifier = byteStream.read<uint32>();
				fontAssetDescriptor.version = byteStream.read<VersionNumber>();

				const uint32 glyphMapImageAssetIdentifier = byteStream.read<uint32>();

				fontAssetDescriptor.glyphMapImage = getAsset<ImageAsset>(glyphMapImageAssetIdentifier, path);

				const sint16 ascent = byteStream.read<sint16>();
				const sint16 descent = byteStream.read<sint16>();
				const sint16 lineGap = byteStream.read<sint16>();

				fontAssetDescriptor.lineHeight = ascent - descent + lineGap;

				byteStream.jump(sizeof(uint16));

				const uint32 glyphCount = byteStream.read<uint32>();

				fontAssetDescriptor.glyphs = sequence<Glyph>(glyphCount);

				for (Glyph& glyph : fontAssetDescriptor.glyphs) {
					glyph.minCoordinate.x = byteStream.read<float32>();
					glyph.minCoordinate.y = byteStream.read<float32>();
					glyph.maxCoordinate.x = byteStream.read<float32>();
					glyph.maxCoordinate.y = byteStream.read<float32>();
					glyph.whitespace = glyph.minCoordinate.x == glyph.maxCoordinate.x && glyph.minCoordinate.y == glyph.maxCoordinate.y;
				}

				for (Glyph& glyph : fontAssetDescriptor.glyphs) {
					glyph.width = byteStream.read<uint16>();
				}

				for (Glyph& glyph : fontAssetDescriptor.glyphs) {
					glyph.advanceWidth = byteStream.read<uint16>();
				}

				for (Glyph& glyph : fontAssetDescriptor.glyphs) {
					glyph.height = byteStream.read<uint16>();
				}

				for (Glyph& glyph : fontAssetDescriptor.glyphs) {
					glyph.leftSideBearing = byteStream.read<sint16>();
				}

				for (Glyph& glyph : fontAssetDescriptor.glyphs) {
					glyph.topSideBearing = byteStream.read<sint16>();
				}

				const uint64 unicodeMapOffset = byteStream.getByteOffset();
				const uint64 unicodeMapSize = byteStream.getRemainingSize() / (sizeof(uint16) + sizeof(uint32));

				fontAssetDescriptor.unicodeMap.reserve(unicodeMapSize);

				for (uint64 unicodeEntryIndex = 0; unicodeEntryIndex < unicodeMapSize; unicodeEntryIndex += 1) {
					byteStream.seek(unicodeMapOffset + unicodeMapSize * sizeof(uint16) + unicodeEntryIndex * sizeof(uint32));

					const uint32 glyphIndex = byteStream.peek<uint32>();

					byteStream.seek(unicodeMapOffset + unicodeEntryIndex * sizeof(uint16));

					const uint16 codePoint = byteStream.read<uint16>();

					fontAssetDescriptor.unicodeMap.insert({codePoint, glyphIndex});
				}

				return createRef<FontAsset>(fontAssetDescriptor);
			}

			ref<ImageAsset> decodeImageAsset(ByteStream<StreamOperation::READ>& byteStream) const {
				const ImageAssetHeader& imageAssetHeader = *pointer_cast<const ImageAssetHeader*>(byteStream.getBytePosition());

				byteStream.jump(sizeof(imageAssetHeader));

				ImageAssetDescriptor imageAssetDescriptor{};
				imageAssetDescriptor.identifier = imageAssetHeader.identifier;
				imageAssetDescriptor.version = imageAssetHeader.version;
				imageAssetDescriptor.format = static_cast<ImageFormat>(imageAssetHeader.format);
				imageAssetDescriptor.extent = imageAssetHeader.extent;
				imageAssetDescriptor.mipLevelCount = imageAssetHeader.mipLevelCount;
				imageAssetDescriptor.layerCount = imageAssetHeader.layerCount;

				const uint8 channelCount = getChannelCount(imageAssetDescriptor.format);

				imageAssetDescriptor.data = sequence<uint8>(imageAssetDescriptor.extent.width * imageAssetDescriptor.extent.height * imageAssetDescriptor.extent.depth * imageAssetDescriptor.layerCount * channelCount);

				switch (imageAssetDescriptor.format) {
					case ImageFormat::R8_UNORM: {
						std::copy_n(byteStream.getBytePosition(), imageAssetDescriptor.data.count(), &imageAssetDescriptor.data[0]);

						byteStream.jump(imageAssetDescriptor.data.count());

						break;
					}
					case ImageFormat::R8G8B8_UNORM:
					case ImageFormat::R8G8B8A8_UNORM: {
						Filesystem::QOIDecoderContentOnly(byteStream, imageAssetDescriptor.data, channelCount, 0);

						break;
					}
					default: {
						ENGINE_ERROR_UNSUPPORTED();

						break;
					}
				}

				return createRef<ImageAsset>(imageAssetDescriptor);
			}

			ref<Scene2DAsset> decodeScene2DAsset(ByteStream<StreamOperation::READ>& stream) const {
				Scene2DAssetDescriptor sceneAssetDescriptor{};
				sceneAssetDescriptor.identifier = stream.read<uint32>();
				sceneAssetDescriptor.version = stream.read<VersionNumber>();
				sceneAssetDescriptor.staticVertexCount = stream.read<uint32>();
				sceneAssetDescriptor.staticIndexCount = stream.read<uint32>();
				sceneAssetDescriptor.dynamicVertexCount = stream.read<uint32>();
				sceneAssetDescriptor.dynamicIndexCount = stream.read<uint32>();
				sceneAssetDescriptor.entityCount = stream.read<uint32>();

				const uint64 vertexCount = (sceneAssetDescriptor.staticVertexCount + sceneAssetDescriptor.dynamicVertexCount) * 2;
				const uint64 indexCount = sceneAssetDescriptor.staticIndexCount + sceneAssetDescriptor.dynamicIndexCount;

				sceneAssetDescriptor.vertexAndIndexBuffer = sequence<uint8>(vertexCount * sizeof(float32) + indexCount * sizeof(uint16));
				sceneAssetDescriptor.geometries = sequence<CachedGeometry2D>(stream.read<uint32>());
				sceneAssetDescriptor.rigidBodies = sequence<CachedRigidBody2D>(stream.read<uint32>());
				sceneAssetDescriptor.geometryComponents = sequence<GeometryComponent>(stream.read<uint32>());

				std::copy_n(stream.getBytePosition(), sceneAssetDescriptor.vertexAndIndexBuffer.count(), &sceneAssetDescriptor.vertexAndIndexBuffer[0]);
				stream.jump(sceneAssetDescriptor.vertexAndIndexBuffer.count());

				for (CachedGeometry2D& geometry : sceneAssetDescriptor.geometries) {
					geometry.centroid = stream.read<svec2<float32>>();
					geometry.radius = stream.read<float32>();
					geometry.entity = stream.read<uint32>();
					geometry.vertexOffset = stream.read<uint32>();
					geometry.vertexCount = stream.read<uint32>();
					geometry.indexOffset = stream.read<uint32>();
					geometry.triangleListIndexCount = stream.read<uint16>();
					geometry.lineStripIndexCount = stream.read<uint16>();
					geometry.type = stream.read<Geometry2DType>();
				}

				for (CachedRigidBody2D& rigidBody : sceneAssetDescriptor.rigidBodies) {
					rigidBody.position = stream.read<svec2<float32>>();
					rigidBody.scale = stream.read<svec2<float32>>();
					rigidBody.rotation = stream.read<float32>();
					rigidBody.inverseMass = stream.read<float32>();
					rigidBody.inverseInertia = stream.read<float32>();
					rigidBody.friction = stream.read<float32>();
					rigidBody.entity = stream.read<uint32>();
				}

				for (GeometryComponent& geometryComponent : sceneAssetDescriptor.geometryComponents) {
					geometryComponent = stream.read<GeometryComponent>();
				}

				return createRef<Scene2DAsset>(sceneAssetDescriptor);
			}

			ref<ShaderModuleAsset> decodeShaderModuleAsset(ByteStream<StreamOperation::READ>& byteStream) const {
				ShaderModuleAssetDescriptor shaderModuleAssetDescriptor{};
				shaderModuleAssetDescriptor.identifier = byteStream.read<uint32>();
				shaderModuleAssetDescriptor.version = byteStream.read<VersionNumber>();
				shaderModuleAssetDescriptor.glslCode = sequence<char8>(byteStream.read<uint32>());

				for (char8& character : shaderModuleAssetDescriptor.glslCode) {
					character = byteStream.read<char8>();
				}

				const uint32 entryPointCount = byteStream.read<uint32>();

				shaderModuleAssetDescriptor.glslEntryPointActivationOffsets.reserve(entryPointCount);

				for (uint32 entryPointIndex = 0; entryPointIndex < entryPointCount; entryPointIndex += 1) {
					const uint32 identifierSize = byteStream.read<uint32>();
					const view<char8> identifier(pointer_cast<const char8*>(byteStream.getBytePosition()), identifierSize);

					byteStream.jump(identifierSize);

					const uint32 activationOffset = byteStream.read<uint32>();

					shaderModuleAssetDescriptor.glslEntryPointActivationOffsets.insert({identifier, activationOffset});
				}

				return createRef<ShaderModuleAsset>(shaderModuleAssetDescriptor);
			}

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