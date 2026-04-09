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
			inline static uint32 createAssetIdentifier() {
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
				Descriptor<Scene2DAsset> sceneAssetDescriptor{};
				sceneAssetDescriptor.identifier = stream.read<uint32>();
				sceneAssetDescriptor.version = stream.read<VersionNumber>();
				sceneAssetDescriptor.staticVertexCount = stream.read<uint32>();
				sceneAssetDescriptor.staticIndexCount = stream.read<uint32>();
				sceneAssetDescriptor.dynamicVertexPositionCount = 0;
				sceneAssetDescriptor.dynamicVertexNormalCount = 0;
				sceneAssetDescriptor.dynamicVertexTextureCoordinateCount = 0;
				sceneAssetDescriptor.dynamicVertexPositionIndexCount = 0;
				sceneAssetDescriptor.dynamicVertexNormalIndexCount = 0;
				sceneAssetDescriptor.dynamicVertexTextureCoordinateIndexCount = 0;
				sceneAssetDescriptor.entityCount = stream.read<uint32>();
				sceneAssetDescriptor.hullCount = 0;
				sceneAssetDescriptor.staticBuffer = sequence<uint8>(sceneAssetDescriptor.staticVertexCount * sizeof(svec2<float32>) + sceneAssetDescriptor.staticIndexCount * sizeof(uint16));

				const uint32 geometryAssetCount = stream.read<uint32>();

				sceneAssetDescriptor.geometryAssets = sequence<ref<GeometryAsset>>(geometryAssetCount);

				const uint32 geometryMappingCount = stream.read<uint32>();

				sceneAssetDescriptor.geometryMappings = sequence<EntityComponentMapping<GeometryComponent>>(geometryMappingCount);

				const uint32 rigidBodyMappingCount = stream.read<uint32>();

				sceneAssetDescriptor.rigidBodyMappings = sequence<EntityComponentMapping<RigidBody2DComponent>>(rigidBodyMappingCount);

				std::copy_n(stream.getBytePosition(), sceneAssetDescriptor.staticBuffer.count(), &sceneAssetDescriptor.staticBuffer[0]);

				stream.jump(sceneAssetDescriptor.staticBuffer.count());

				for (ref<GeometryAsset>& geometryAsset : sceneAssetDescriptor.geometryAssets) {
					const uint32 identifier = stream.read<uint32>();

					geometryAsset = getAsset<GeometryAsset>(identifier);

					sceneAssetDescriptor.dynamicVertexPositionCount += geometryAsset->getVertexPositionComponents().count() / 2;
					sceneAssetDescriptor.dynamicVertexTextureCoordinateCount += geometryAsset->getVertexTextureCoordinateComponents().count() / 2;
					sceneAssetDescriptor.dynamicVertexPositionIndexCount += geometryAsset->getVertexPositionIndices().count();
					sceneAssetDescriptor.dynamicVertexTextureCoordinateIndexCount += geometryAsset->getVertexTextureCoordinateIndices().count();

					if (geometryAsset->getFlags() & GeometryAssetFlags::HAS_HULL) {
						sceneAssetDescriptor.dynamicVertexPositionIndexCount += geometryAsset->getHullLineIndices().count();
						sceneAssetDescriptor.hullCount += 1;
					}
				}

				for (EntityComponentMapping<GeometryComponent>& geometryMapping : sceneAssetDescriptor.geometryMappings) {
					geometryMapping.entity = stream.read<Entity>();
					geometryMapping.component.geometryIndex = stream.read<uint32>();
				}

				for (EntityComponentMapping<RigidBody2DComponent>& rigidBodyMapping : sceneAssetDescriptor.rigidBodyMappings) {
					rigidBodyMapping.entity = stream.read<Entity>();
					rigidBodyMapping.component.position = stream.read<svec2<float32>>();
					rigidBodyMapping.component.rotation = stream.read<float32>();
					rigidBodyMapping.component.scale = stream.read<svec2<float32>>();
					rigidBodyMapping.component.linearVelocity = stream.read<svec2<float32>>();
					rigidBodyMapping.component.angularVelocity = stream.read<float32>();
					rigidBodyMapping.component.inverseMass = stream.read<float32>();
					rigidBodyMapping.component.inverseInertia = stream.read<float32>();
					rigidBodyMapping.component.friction = stream.read<float32>();
					rigidBodyMapping.component.restitution = stream.read<float32>();
				}

				return createRef<Scene2DAsset>(sceneAssetDescriptor);
			}

			ref<Scene3DAsset> decodeScene3DAsset(ByteStream<StreamOperation::READ>& stream) const {
				Descriptor<Scene3DAsset> sceneAssetDescriptor{};
				sceneAssetDescriptor.identifier = stream.read<uint32>();
				sceneAssetDescriptor.version = stream.read<VersionNumber>();
				sceneAssetDescriptor.staticVertexCount = stream.read<uint32>();
				sceneAssetDescriptor.staticIndexCount = stream.read<uint32>();
				sceneAssetDescriptor.dynamicVertexPositionCount = 0;
				sceneAssetDescriptor.dynamicVertexNormalCount = 0;
				sceneAssetDescriptor.dynamicVertexTextureCoordinateCount = 0;
				sceneAssetDescriptor.dynamicVertexPositionIndexCount = 0;
				sceneAssetDescriptor.dynamicVertexNormalIndexCount = 0;
				sceneAssetDescriptor.dynamicVertexTextureCoordinateIndexCount = 0;
				sceneAssetDescriptor.entityCount = stream.read<uint32>();
				sceneAssetDescriptor.hullCount = 0;
				sceneAssetDescriptor.staticBuffer = sequence<uint8>(sceneAssetDescriptor.staticVertexCount * sizeof(svec2<float32>) + sceneAssetDescriptor.staticIndexCount * sizeof(uint16));

				const uint32 geometryAssetCount = stream.read<uint32>();

				sceneAssetDescriptor.geometryAssets = sequence<ref<GeometryAsset>>(geometryAssetCount);

				const uint32 geometryMappingCount = stream.read<uint32>();

				sceneAssetDescriptor.geometryMappings = sequence<EntityComponentMapping<GeometryComponent>>(geometryMappingCount);

				const uint32 rigidBodyMappingCount = stream.read<uint32>();

				sceneAssetDescriptor.rigidBodyMappings = sequence<EntityComponentMapping<RigidBody3DComponent>>(rigidBodyMappingCount);

				std::copy_n(stream.getBytePosition(), sceneAssetDescriptor.staticBuffer.count(), &sceneAssetDescriptor.staticBuffer[0]);

				stream.jump(sceneAssetDescriptor.staticBuffer.count());

				for (ref<GeometryAsset>& geometryAsset : sceneAssetDescriptor.geometryAssets) {
					const uint32 identifier = stream.read<uint32>();

					geometryAsset = getAsset<GeometryAsset>(identifier);

					sceneAssetDescriptor.dynamicVertexPositionCount += geometryAsset->getVertexPositionComponents().count() / 3;
					sceneAssetDescriptor.dynamicVertexNormalCount += geometryAsset->getVertexNormalComponents().count() / 3;
					sceneAssetDescriptor.dynamicVertexTextureCoordinateCount += geometryAsset->getVertexTextureCoordinateComponents().count() / 2;
					sceneAssetDescriptor.dynamicVertexPositionIndexCount += geometryAsset->getVertexPositionIndices().count();
					sceneAssetDescriptor.dynamicVertexNormalIndexCount += geometryAsset->getVertexNormalIndices().count();
					sceneAssetDescriptor.dynamicVertexTextureCoordinateIndexCount += geometryAsset->getVertexTextureCoordinateIndices().count();

					if (geometryAsset->getFlags() & GeometryAssetFlags::HAS_HULL) {
						sceneAssetDescriptor.dynamicVertexPositionIndexCount += geometryAsset->getHullLineIndices().count();
						sceneAssetDescriptor.hullCount += 1;
					}
				}

				for (EntityComponentMapping<GeometryComponent>& geometryMapping : sceneAssetDescriptor.geometryMappings) {
					geometryMapping.entity = stream.read<Entity>();
					geometryMapping.component.geometryIndex = stream.read<uint32>();
				}

				for (EntityComponentMapping<RigidBody3DComponent>& rigidBodyMapping : sceneAssetDescriptor.rigidBodyMappings) {
					rigidBodyMapping.entity = stream.read<Entity>();

					const svec3<float32> position = stream.read<svec3<float32>>();
					const svec3<float32> rotation = stream.read<svec3<float32>>();
					const svec3<float32> scale = stream.read<svec3<float32>>();

					rigidBodyMapping.component.position = {position[0], position[1], position[2]};
					rigidBodyMapping.component.rotation = {rotation[0], rotation[1], rotation[2]};
					rigidBodyMapping.component.scale = {scale[0], scale[1], scale[2]};
				}

				return createRef<Scene3DAsset>(sceneAssetDescriptor);
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

	template<>
	inline const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<FontAsset>() const {
		return fontAssetPathToIndex;
	}

	template<>
	inline const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<GeometryAsset>() const {
		return geometryAssetPathToIndex;
	}

	template<>
	inline const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<ImageAsset>() const {
		return imageAssetPathToIndex;
	}

	// 2D and 3D scenes share the same index map.
	template<>
	inline const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<Scene2DAsset>() const {
		return sceneAssetPathToIndex;
	}

	// 2D and 3D scenes share the same index map.
	template<>
	inline const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<Scene3DAsset>() const {
		return sceneAssetPathToIndex;
	}

	template<>
	inline const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<ShaderModuleAsset>() const {
		return shaderModuleAssetPathToIndex;
	}

	template<>
	inline void AssetLibrary::loadAsset<FontAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Font", path, ".tfo");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> byteStream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		if (byteStream.getRemainingSize() < sizeof(AssetSignature) || byteStream.peek<AssetSignature>() != AssetSignature::FONT) {
			return;
		}

		byteStream.jump(sizeof(AssetSignature));

		const ref<FontAsset> fontAsset = decodeFontAsset(path, byteStream);
		const uint32 assetIndex = static_cast<uint32>(assets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({fontAsset->getIdentifier(), assetIndex});
		fontAssetPathToIndex.insert({stem, assetIndex});
		assets.emplace_back(fontAsset);
	}

	template<>
	inline void AssetLibrary::loadAsset<GeometryAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Geometry", path, ".tga");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> byteStream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		if (byteStream.getRemainingSize() < sizeof(AssetSignature) || byteStream.peek<AssetSignature>() != AssetSignature::GEOMETRY) {
			return;
		}

		byteStream.jump(sizeof(AssetSignature));

		const ref<GeometryAsset> geometryAsset = Filesystem::decodeAsset<GeometryAsset>(byteStream);
		const uint32 assetIndex = static_cast<uint32>(assets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({geometryAsset->getIdentifier(), assetIndex});
		geometryAssetPathToIndex.insert({stem, assetIndex});
		assets.emplace_back(geometryAsset);
	}

	template<>
	inline void AssetLibrary::loadAsset<ImageAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Image", path, ".tia");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> byteStream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		if (byteStream.getRemainingSize() < sizeof(AssetSignature) || byteStream.peek<AssetSignature>() != AssetSignature::IMAGE) {
			return;
		}

		const ref<ImageAsset> imageAsset = decodeImageAsset(byteStream);
		const uint32 assetIndex = static_cast<uint32>(assets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({imageAsset->getIdentifier(), assetIndex});
		imageAssetPathToIndex.insert({stem, assetIndex});
		assets.emplace_back(imageAsset);
	}

	template<>
	inline void AssetLibrary::loadAsset<Scene2DAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Scene", path, ".tsa");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> byteStream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		if (byteStream.getRemainingSize() < sizeof(AssetSignature) || byteStream.peek<AssetSignature>() != AssetSignature::SCENE) {
			return;
		}

		byteStream.jump(sizeof(AssetSignature));

		const ref<Scene2DAsset> sceneAsset = decodeScene2DAsset(byteStream);
		const uint32 assetIndex = static_cast<uint32>(assets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({sceneAsset->getIdentifier(), assetIndex});
		sceneAssetPathToIndex.insert({stem, assetIndex});
		assets.emplace_back(sceneAsset);
	}

	template<>
	inline void AssetLibrary::loadAsset<Scene3DAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Scene", path, ".tsa");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> byteStream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		if (byteStream.getRemainingSize() < sizeof(AssetSignature) || byteStream.peek<AssetSignature>() != AssetSignature::SCENE) {
			return;
		}

		byteStream.jump(sizeof(AssetSignature));

		const ref<Scene3DAsset> sceneAsset = decodeScene3DAsset(byteStream);
		const uint32 assetIndex = static_cast<uint32>(assets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({sceneAsset->getIdentifier(), assetIndex});
		sceneAssetPathToIndex.insert({stem, assetIndex});
		assets.emplace_back(sceneAsset);
	}

	template<>
	inline void AssetLibrary::loadAsset<ShaderModuleAsset>(view<char8> path) {
		const std::filesystem::path absolutePath = getAbsolutePath("Asset/Shader", path, ".tsm");
		const sequence<char8> assetPath = absolutePath.string();
		const sequence<uint8> data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> byteStream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		if (byteStream.getRemainingSize() < sizeof(AssetSignature) || byteStream.peek<AssetSignature>() != AssetSignature::SHADER_MODULE) {
			return;
		}

		byteStream.jump(sizeof(AssetSignature));

		const ref<ShaderModuleAsset> shaderModuleAsset = decodeShaderModuleAsset(byteStream);
		const uint32 assetIndex = static_cast<uint32>(assets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({shaderModuleAsset->getIdentifier(), assetIndex});
		shaderModuleAssetPathToIndex.insert({stem, assetIndex});
		assets.emplace_back(shaderModuleAsset);
	}
}