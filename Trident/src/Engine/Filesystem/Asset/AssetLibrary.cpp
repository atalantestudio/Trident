// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Asset/AssetSignature.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	std::filesystem::path AssetLibrary::getAbsolutePath(view<char8> base, view<char8> relativePath, view<char8> extension) {
		sequence<char8> path(relativePath.count() + extension.count());

		std::copy(relativePath.begin(), relativePath.end(), &path[0]);
		std::copy(extension.begin(), extension.end(), &path[relativePath.count()]);

		return std::filesystem::path(base.begin(), base.end()) / std::filesystem::path(path.begin(), path.end());
	}

	ref<FontAsset> AssetLibrary::decodeFontAsset(view<char8> path, ByteStream<StreamOperation::READ>& stream) {
		FontAssetDescriptor fontAssetDescriptor{};
		fontAssetDescriptor.identifier = stream.read<uint32>();
		fontAssetDescriptor.version = stream.read<VersionNumber>();

		const uint32 glyphMapImageAssetIdentifier = stream.read<uint32>();

		fontAssetDescriptor.glyphMapImage = getAsset<ImageAsset>(glyphMapImageAssetIdentifier, path);

		const sint16 ascent = stream.read<sint16>();
		const sint16 descent = stream.read<sint16>();
		const sint16 lineGap = stream.read<sint16>();

		fontAssetDescriptor.lineHeight = ascent - descent + lineGap;

		stream.jump(sizeof(uint16));

		const uint32 glyphCount = stream.read<uint32>();
		const uint32 unicodeEntryCount = stream.read<uint32>();
		const uint32 kerningPairCount = stream.read<uint32>();

		fontAssetDescriptor.glyphs = sequence<Glyph>(glyphCount);

		for (Glyph& glyph : fontAssetDescriptor.glyphs) {
			glyph.minCoordinate.x = stream.read<float32>();
			glyph.minCoordinate.y = stream.read<float32>();
			glyph.maxCoordinate.x = stream.read<float32>();
			glyph.maxCoordinate.y = stream.read<float32>();
			glyph.whitespace = glyph.minCoordinate.x == glyph.maxCoordinate.x && glyph.minCoordinate.y == glyph.maxCoordinate.y;
		}

		for (Glyph& glyph : fontAssetDescriptor.glyphs) {
			glyph.width = stream.read<uint16>();
		}

		for (Glyph& glyph : fontAssetDescriptor.glyphs) {
			glyph.advanceWidth = stream.read<uint16>();
		}

		for (Glyph& glyph : fontAssetDescriptor.glyphs) {
			glyph.height = stream.read<uint16>();
		}

		for (Glyph& glyph : fontAssetDescriptor.glyphs) {
			glyph.leftSideBearing = stream.read<sint16>();
		}

		for (Glyph& glyph : fontAssetDescriptor.glyphs) {
			glyph.topSideBearing = stream.read<sint16>();
		}

		ByteStream<StreamOperation::READ> keyStream(stream.getBytePosition(), stream.getRemainingSize(), stream.getEndianness());
		ByteStream<StreamOperation::READ> valueStream(stream.getBytePosition(), stream.getRemainingSize(), stream.getEndianness());

		valueStream.jump(unicodeEntryCount * sizeof(uint16));

		fontAssetDescriptor.unicodeMap.reserve(unicodeEntryCount);

		uint16 entryIndex = 0;

		while (entryIndex < unicodeEntryCount) {
			const uint16 codePoint = keyStream.read<uint16>();
			const uint32 glyphIndex = valueStream.read<uint32>();

			fontAssetDescriptor.unicodeMap.insert({codePoint, glyphIndex});

			entryIndex += 1;
		}

		fontAssetDescriptor.kerningPairs.reserve(kerningPairCount);

		entryIndex = 0;

		while (entryIndex < kerningPairCount) {
			const uint16 firstGlyphIndex = keyStream.read<uint16>();
			const uint16 secondGlyphIndex = keyStream.read<uint16>();
			const GlyphPair glyphPair = {firstGlyphIndex, secondGlyphIndex};
			const uint32 firstGlyphXAdvance = valueStream.read<sint16>();

			fontAssetDescriptor.kerningPairs.insert({glyphPair, firstGlyphXAdvance});

			entryIndex += 1;
		}

		return createRef<FontAsset>(fontAssetDescriptor);
	}

	ref<ImageAsset> AssetLibrary::decodeImageAsset(ByteStream<StreamOperation::READ>& byteStream) const {
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

	ref<Scene2DAsset> AssetLibrary::decodeScene2DAsset(ByteStream<StreamOperation::READ>& stream) const {
		Descriptor<Scene2DAsset> sceneAssetDescriptor{};
		sceneAssetDescriptor.identifier = stream.read<uint32>();
		sceneAssetDescriptor.version = stream.read<VersionNumber>();
		sceneAssetDescriptor.staticVertexCount = stream.read<uint32>();
		sceneAssetDescriptor.staticIndexCount = stream.read<uint32>();
		sceneAssetDescriptor.dynamicVertexPositionCount = 0;
		sceneAssetDescriptor.dynamicVertexTextureCoordinateCount = 0;
		sceneAssetDescriptor.dynamicVertexPositionIndexCount = 0;
		sceneAssetDescriptor.dynamicVertexTextureCoordinateIndexCount = 0;
		sceneAssetDescriptor.entityCount = stream.read<uint32>();
		sceneAssetDescriptor.hullCount = 0;
		sceneAssetDescriptor.staticBuffer = sequence<uint8>(sceneAssetDescriptor.staticVertexCount * sizeof(svec2<float32>) + sceneAssetDescriptor.staticIndexCount * sizeof(uint16));

		const uint32 geometryAssetCount = stream.read<uint32>();
		const uint32 geometryMappingCount = stream.read<uint32>();
		const uint32 materialMappingCount = stream.read<uint32>();
		const uint32 rigidBodyMappingCount = stream.read<uint32>();

		sceneAssetDescriptor.geometryAssets = sequence<ref<GeometryAsset>>(geometryAssetCount);
		sceneAssetDescriptor.geometryMappings = sequence<EntityComponentMapping<GeometryComponent>>(geometryMappingCount);
		sceneAssetDescriptor.materialMappings = sequence<EntityComponentMapping<MaterialComponent>>(materialMappingCount);
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
				// TODO: Don't mix triangle and line indices?
				sceneAssetDescriptor.dynamicVertexPositionIndexCount += geometryAsset->getHull2DLineIndices().count();

				sceneAssetDescriptor.hullCount += 1;
			}
		}

		for (EntityComponentMapping<GeometryComponent>& geometryMapping : sceneAssetDescriptor.geometryMappings) {
			geometryMapping.entity = stream.read<Entity>();
			geometryMapping.component.geometryIndex = stream.read<uint32>();
		}

		for (EntityComponentMapping<MaterialComponent>& materialMapping : sceneAssetDescriptor.materialMappings) {
			materialMapping.entity = stream.read<Entity>();
			materialMapping.component.materialIndex = stream.read<uint32>();
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

	ref<Scene3DAsset> AssetLibrary::decodeScene3DAsset(ByteStream<StreamOperation::READ>& stream) const {
		Descriptor<Scene3DAsset> sceneAssetDescriptor{};
		sceneAssetDescriptor.identifier = stream.read<uint32>();
		sceneAssetDescriptor.version = stream.read<VersionNumber>();
		sceneAssetDescriptor.staticVertexCount = stream.read<uint32>();
		sceneAssetDescriptor.staticIndexCount = stream.read<uint32>();
		sceneAssetDescriptor.dynamicVertexPositionCount = 0;
		sceneAssetDescriptor.dynamicVertexNormalCount = 0;
		sceneAssetDescriptor.dynamicVertexTextureCoordinateCount = 0;
		sceneAssetDescriptor.dynamicTriangleTangentCount = 0;
		sceneAssetDescriptor.dynamicVertexPositionIndexCount = 0;
		sceneAssetDescriptor.dynamicVertexNormalIndexCount = 0;
		sceneAssetDescriptor.dynamicVertexTextureCoordinateIndexCount = 0;
		sceneAssetDescriptor.entityCount = stream.read<uint32>();
		sceneAssetDescriptor.hullCount = 0;
		sceneAssetDescriptor.hullEdgeCount = 0;
		sceneAssetDescriptor.hullFaceCount = 0;
		sceneAssetDescriptor.hullFaceEdgeIndexCount = 0;
		sceneAssetDescriptor.rigidBodyFacePlaneCount = 0;
		sceneAssetDescriptor.staticBuffer = sequence<uint8>(sceneAssetDescriptor.staticVertexCount * sizeof(svec2<float32>) + sceneAssetDescriptor.staticIndexCount * sizeof(uint16));

		const uint32 geometryAssetCount = stream.read<uint32>();
		const uint32 geometryMappingCount = stream.read<uint32>();
		const uint32 materialMappingCount  = stream.read<uint32>();
		const uint32 rigidBodyMappingCount = stream.read<uint32>();

		sceneAssetDescriptor.geometryAssets = sequence<ref<GeometryAsset>>(geometryAssetCount);
		sceneAssetDescriptor.geometryMappings = sequence<EntityComponentMapping<GeometryComponent>>(geometryMappingCount);
		sceneAssetDescriptor.materialMappings = sequence<EntityComponentMapping<MaterialComponent>>(materialMappingCount);
		sceneAssetDescriptor.rigidBodyMappings = sequence<EntityComponentMapping<RigidBody3DComponent>>(rigidBodyMappingCount);

		std::copy_n(stream.getBytePosition(), sceneAssetDescriptor.staticBuffer.count(), &sceneAssetDescriptor.staticBuffer[0]);

		stream.jump(sceneAssetDescriptor.staticBuffer.count());

		for (ref<GeometryAsset>& geometryAsset : sceneAssetDescriptor.geometryAssets) {
			const uint32 identifier = stream.read<uint32>();

			geometryAsset = getAsset<GeometryAsset>(identifier);

			sceneAssetDescriptor.dynamicVertexPositionCount += geometryAsset->getVertexPositionComponents().count() / 3;
			sceneAssetDescriptor.dynamicVertexNormalCount += geometryAsset->getVertexNormalComponents().count() / 3;
			sceneAssetDescriptor.dynamicVertexTextureCoordinateCount += geometryAsset->getVertexTextureCoordinateComponents().count() / 2;
			sceneAssetDescriptor.dynamicTriangleTangentCount += geometryAsset->getTriangleTangentComponents().count() / 3;
			sceneAssetDescriptor.dynamicVertexPositionIndexCount += geometryAsset->getVertexPositionIndices().count();
			sceneAssetDescriptor.dynamicVertexNormalIndexCount += geometryAsset->getVertexNormalIndices().count();
			sceneAssetDescriptor.dynamicVertexTextureCoordinateIndexCount += geometryAsset->getVertexTextureCoordinateIndices().count();

			if (geometryAsset->getFlags() & GeometryAssetFlags::HAS_HULL) {
				sceneAssetDescriptor.hullCount += 1;
				sceneAssetDescriptor.hullEdgeCount += static_cast<uint32>(geometryAsset->getHull3DEdges().count());
				sceneAssetDescriptor.hullFaceCount += static_cast<uint32>(geometryAsset->getHull3DFaces().count());
				sceneAssetDescriptor.hullFaceEdgeIndexCount += static_cast<uint16>(geometryAsset->getHull3DFaceEdgeIndices().count());
				sceneAssetDescriptor.hullLineIndexCount += static_cast<uint32>(geometryAsset->getHull3DLineIndices().count());
			}
		}

		for (EntityComponentMapping<GeometryComponent>& geometryMapping : sceneAssetDescriptor.geometryMappings) {
			geometryMapping.entity = stream.read<Entity>();
			geometryMapping.component.geometryIndex = stream.read<uint32>();
		}

		for (EntityComponentMapping<MaterialComponent>& materialMapping : sceneAssetDescriptor.materialMappings) {
			materialMapping.entity = stream.read<Entity>();
			materialMapping.component.materialIndex = stream.read<uint32>();
		}

		for (EntityComponentMapping<RigidBody3DComponent>& rigidBodyMapping : sceneAssetDescriptor.rigidBodyMappings) {
			rigidBodyMapping.entity = stream.read<Entity>();
			rigidBodyMapping.component.position = vec3<float32>(stream.read<svec3<float32>>());
			rigidBodyMapping.component.rotation = vec3<float32>(stream.read<svec3<float32>>());
			rigidBodyMapping.component.linearVelocity = vec3<float32>(stream.read<svec3<float32>>());
			rigidBodyMapping.component.angularVelocity = vec3<float32>(stream.read<svec3<float32>>());
			rigidBodyMapping.component.scale = stream.read<float32>();
			rigidBodyMapping.component.inverseMass = stream.read<float32>();
			rigidBodyMapping.component.inverseInertia = {
				vec3<float32>(stream.read<svec3<float32>>()),
				vec3<float32>(stream.read<svec3<float32>>()),
				vec3<float32>(stream.read<svec3<float32>>()),
			};
			rigidBodyMapping.component.friction = stream.read<float32>();
			rigidBodyMapping.component.restitution = stream.read<float32>();

			for (const EntityComponentMapping<GeometryComponent>& geometryMapping : sceneAssetDescriptor.geometryMappings) {
				if (geometryMapping.entity != rigidBodyMapping.entity) {
					continue;
				}

				const ref<GeometryAsset>& geometryAsset = sceneAssetDescriptor.geometryAssets[geometryMapping.component.geometryIndex];

				if (!(geometryAsset->getFlags() & GeometryAssetFlags::HAS_HULL)) {
					continue;
				}

				sceneAssetDescriptor.rigidBodyFacePlaneCount += static_cast<uint32>(geometryAsset->getHull3DFaces().count());
			}
		}

		return createRef<Scene3DAsset>(sceneAssetDescriptor);
	}

	ref<ShaderModuleAsset> AssetLibrary::decodeShaderModuleAsset(ByteStream<StreamOperation::READ>& byteStream) const {
		ShaderModuleAssetDescriptor shaderModuleAssetDescriptor{};
		shaderModuleAssetDescriptor.identifier = byteStream.read<uint32>();
		shaderModuleAssetDescriptor.version = byteStream.read<VersionNumber>();
		shaderModuleAssetDescriptor.glslCode = sequence<char8>(byteStream.read<uint32>());

		for (char8& character : shaderModuleAssetDescriptor.glslCode) {
			character = byteStream.read<char8>();
		}

		const uint32 entryPointCount = byteStream.read<uint32>();

		shaderModuleAssetDescriptor.glslEntryPointActivationOffsets.reserve(entryPointCount);

		uint32 entryPointIndex = 0;

		sequence<view<char8>> entryPointIdentifiers(entryPointCount);

		while (entryPointIndex < entryPointCount) {
			const uint32 identifierSize = byteStream.read<uint32>();

			entryPointIdentifiers[entryPointIndex] = view<char8>(pointer_cast<const char8*>(byteStream.getBytePosition()), identifierSize);

			byteStream.jump(identifierSize);

			entryPointIndex += 1;
		}

		entryPointIndex = 0;

		while (entryPointIndex < entryPointCount) {
			const uint32 activationOffset = byteStream.read<uint32>();

			shaderModuleAssetDescriptor.glslEntryPointActivationOffsets.insert({entryPointIdentifiers[entryPointIndex], activationOffset});

			entryPointIndex += 1;
		}

		return createRef<ShaderModuleAsset>(shaderModuleAssetDescriptor);
	}

	template<>
	const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<FontAsset>() const {
		return fontAssetPathToIndex;
	}

	template<>
	const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<GeometryAsset>() const {
		return geometryAssetPathToIndex;
	}

	template<>
	const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<ImageAsset>() const {
		return imageAssetPathToIndex;
	}

	// 2D and 3D scenes share the same index map.
	template<>
	const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<Scene2DAsset>() const {
		return sceneAssetPathToIndex;
	}

	// 2D and 3D scenes share the same index map.
	template<>
	const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<Scene3DAsset>() const {
		return sceneAssetPathToIndex;
	}

	template<>
	const std::unordered_map<sequence<char8>, uint32>& AssetLibrary::getAssetPathToIndexMap<ShaderModuleAsset>() const {
		return shaderModuleAssetPathToIndex;
	}

	template<>
	void AssetLibrary::loadAsset<FontAsset>(view<char8> path) {
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
	void AssetLibrary::loadAsset<GeometryAsset>(view<char8> path) {
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
	void AssetLibrary::loadAsset<ImageAsset>(view<char8> path) {
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
	void AssetLibrary::loadAsset<Scene2DAsset>(view<char8> path) {
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
	void AssetLibrary::loadAsset<Scene3DAsset>(view<char8> path) {
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
	void AssetLibrary::loadAsset<ShaderModuleAsset>(view<char8> path) {
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