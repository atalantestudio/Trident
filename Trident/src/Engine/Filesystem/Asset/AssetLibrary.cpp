// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Asset/AssetSignature.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	static ref<FontAsset> decodeFontAsset(ByteStream<StreamOperation::READ>& stream, AssetLibrary* assetLibrary, view<char8> path) {
		FontAssetDescriptor descriptor{};
		descriptor.identifier = stream.read<uint32>();
		descriptor.version = stream.read<VersionNumber>();

		const uint32 glyphMapImageAssetIdentifier = stream.read<uint32>();

		descriptor.glyphMapImage = assetLibrary->getAsset<ImageAsset>(glyphMapImageAssetIdentifier, path);

		const sint16 ascent = stream.read<sint16>();
		const sint16 descent = stream.read<sint16>();
		const sint16 lineGap = stream.read<sint16>();

		descriptor.ascent = ascent;
		descriptor.lineHeight = ascent - descent + lineGap;
		descriptor.unitsPerEm = stream.read<uint16>();

		const uint32 glyphCount = stream.read<uint32>();
		const uint32 unicodeEntryCount = stream.read<uint32>();
		const uint32 kerningPairCount = stream.read<uint32>();

		descriptor.glyphs = sequence<Glyph>(glyphCount);

		for (Glyph& glyph : descriptor.glyphs) {
			glyph.minCoordinate.x = stream.read<float32>();
			glyph.minCoordinate.y = stream.read<float32>();
			glyph.maxCoordinate.x = stream.read<float32>();
			glyph.maxCoordinate.y = stream.read<float32>();
			glyph.whitespace = glyph.minCoordinate.x == glyph.maxCoordinate.x && glyph.minCoordinate.y == glyph.maxCoordinate.y;
		}

		for (Glyph& glyph : descriptor.glyphs) {
			glyph.width = stream.read<uint16>();
		}

		for (Glyph& glyph : descriptor.glyphs) {
			glyph.height = stream.read<uint16>();
		}

		for (Glyph& glyph : descriptor.glyphs) {
			glyph.yMax = stream.read<sint16>();
		}

		for (Glyph& glyph : descriptor.glyphs) {
			glyph.advanceWidth = stream.read<uint16>();
		}

		for (Glyph& glyph : descriptor.glyphs) {
			glyph.leftSideBearing = stream.read<sint16>();
		}

		ByteStream<StreamOperation::READ> keyStream(stream.getBytePosition(), stream.getRemainingSize(), stream.getEndianness());
		ByteStream<StreamOperation::READ> valueStream(stream.getBytePosition(), stream.getRemainingSize(), stream.getEndianness());

		valueStream.jump(unicodeEntryCount * sizeof(uint16));

		descriptor.unicodeMap.reserve(unicodeEntryCount);

		uint16 entryIndex = 0;

		while (entryIndex < unicodeEntryCount) {
			const uint16 codePoint = keyStream.read<uint16>();
			const uint32 glyphIndex = valueStream.read<uint32>();

			descriptor.unicodeMap.insert({codePoint, glyphIndex});

			entryIndex += 1;
		}

		descriptor.kerningPairs.reserve(kerningPairCount);

		entryIndex = 0;

		while (entryIndex < kerningPairCount) {
			const uint16 firstGlyphIndex = keyStream.read<uint16>();
			const uint16 secondGlyphIndex = keyStream.read<uint16>();
			const GlyphPair glyphPair = {firstGlyphIndex, secondGlyphIndex};
			const uint32 firstGlyphXAdvance = valueStream.read<sint16>();

			descriptor.kerningPairs.insert({glyphPair, firstGlyphXAdvance});

			entryIndex += 1;
		}

		return createRef<FontAsset>(descriptor);
	}

	AssetLibrary::AssetLibrary(std::filesystem::path basePath) :
		basePath(basePath)
	{}

	void AssetLibrary::loadAssetMap() {
		const std::string projectNameString = basePath.stem().string();
		const view<char8> projectName(&projectNameString[0], projectNameString.size());
		const std::filesystem::path absolutePath = getAbsolutePath("Asset", projectName, ".tam");
		const sequence<char8> assetPath = absolutePath.string();

		assetMap.data = Filesystem::openByteBuffer(assetPath);

		ByteStream<StreamOperation::READ> stream(&assetMap.data[0], assetMap.data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::ASSET_MAP, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature) + sizeof(VersionNumber));

		while (stream.getRemainingSize() > 0) {
			const uint32 identifier = stream.read<uint32>();
			const uint32 pathCharacterCount = stream.read<uint32>();
			const view<char8> path(pointer_cast<const char8*>(stream.getBytePosition()), pathCharacterCount);

			stream.jump(pathCharacterCount);

			assetMap.map.insert({identifier, path});
		}
	}

	std::filesystem::path AssetLibrary::getAbsolutePath(view<char8> base, view<char8> relativePath, view<char8> extension) const {
		sequence<char8> path(relativePath.count() + extension.count());

		std::copy(relativePath.begin(), relativePath.end(), &path[0]);
		std::copy(extension.begin(), extension.end(), &path[relativePath.count()]);

		return basePath / std::filesystem::path(base.begin(), base.end()) / std::filesystem::path(path.begin(), path.end());
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

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::FONT, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<FontAsset> fontAsset = decodeFontAsset(stream, this, path);
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

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::GEOMETRY, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<GeometryAsset> geometryAsset = Filesystem::decodeAsset<GeometryAsset>(stream, this);
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

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::IMAGE, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<ImageAsset> imageAsset = Filesystem::decodeAsset<ImageAsset>(stream, this);
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

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::SCENE, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<Scene2DAsset> sceneAsset = Filesystem::decodeAsset<Scene2DAsset>(stream, this);
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

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::SCENE, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<Scene3DAsset> sceneAsset = Filesystem::decodeAsset<Scene3DAsset>(stream, this);
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

		ByteStream<StreamOperation::READ> stream(&data[0], data.count(), Endianness::LITTLE_ENDIAN);

		ENGINE_ASSERT(stream.getRemainingSize() >= sizeof(AssetSignature), "The file is corrupted.");
		ENGINE_ASSERT(stream.peek<AssetSignature>() == AssetSignature::SHADER_MODULE, "The file format is invalid.");

		stream.jump(sizeof(AssetSignature));

		const ref<ShaderModuleAsset> shaderModuleAsset = Filesystem::decodeAsset<ShaderModuleAsset>(stream, this);
		const uint32 assetIndex = static_cast<uint32>(assets.size());
		const sequence<char8> stem = absolutePath.stem().string();

		assetIdentifierToIndex.insert({shaderModuleAsset->getIdentifier(), assetIndex});
		shaderModuleAssetPathToIndex.insert({stem, assetIndex});
		assets.emplace_back(shaderModuleAsset);
	}
}