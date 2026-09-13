// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Filesystem/Asset/AssetLibrary.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace Trident {
	template<>
	FontAsset Filesystem::decodeAsset(ByteStream<StreamOperation::READ>& stream, AssetLibrary* assetLibrary) {
		FontAsset asset{};
		asset.identifier = stream.read<uint32>();
		asset.version = stream.read<VersionNumber>();

		const uint32 glyphMapImageAssetIdentifier = stream.read<uint32>();

		asset.glyphMapImage = assetLibrary->getAsset<ImageAsset>(glyphMapImageAssetIdentifier);

		const sint16 ascent = stream.read<sint16>();
		const sint16 descent = stream.read<sint16>();
		const sint16 lineGap = stream.read<sint16>();

		asset.ascent = ascent;
		asset.lineHeight = ascent - descent + lineGap;
		asset.unitsPerEm = stream.read<uint16>();

		const uint32 glyphCount = stream.read<uint32>();
		const uint32 unicodeEntryCount = stream.read<uint32>();
		const uint32 kerningPairCount = stream.read<uint32>();

		asset.glyphs = sequence<Glyph>(glyphCount);

		for (Glyph& glyph : asset.glyphs) {
			glyph.minCoordinate.x = stream.read<float32>();
			glyph.minCoordinate.y = stream.read<float32>();
			glyph.maxCoordinate.x = stream.read<float32>();
			glyph.maxCoordinate.y = stream.read<float32>();
			glyph.whitespace = glyph.minCoordinate.x == glyph.maxCoordinate.x && glyph.minCoordinate.y == glyph.maxCoordinate.y;
			glyph.width = stream.read<uint16>();
			glyph.height = stream.read<uint16>();
			glyph.yMax = stream.read<sint16>();
			glyph.advanceWidth = stream.read<uint16>();
			glyph.leftSideBearing = stream.read<sint16>();
		}

		asset.unicodeMap.reserve(unicodeEntryCount);

		uint16 entryIndex = 0;

		while (entryIndex < unicodeEntryCount) {
			const uint16 codePoint = stream.read<uint16>();
			const uint32 glyphIndex = stream.read<uint32>();

			asset.unicodeMap.insert({codePoint, glyphIndex});

			entryIndex += 1;
		}

		asset.kerningPairs.reserve(kerningPairCount);

		entryIndex = 0;

		while (entryIndex < kerningPairCount) {
			const GlyphPair glyphPair = stream.read<GlyphPair>();
			const uint32 firstGlyphXAdvance = stream.read<sint16>();

			asset.kerningPairs.insert({glyphPair, firstGlyphXAdvance});

			entryIndex += 1;
		}

		return asset;
	}
}