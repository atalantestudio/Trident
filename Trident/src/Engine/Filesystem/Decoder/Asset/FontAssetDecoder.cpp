// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	template<>
	ref<FontAsset> Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, AssetLibrary* assetLibrary) {
		FontAssetDescriptor descriptor{};
		descriptor.identifier = stream.read<uint32>();
		descriptor.version = stream.read<VersionNumber>();

		const uint32 glyphMapImageAssetIdentifier = stream.read<uint32>();

		descriptor.glyphMapImage = assetLibrary->getAsset<ImageAsset>(glyphMapImageAssetIdentifier);

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
}