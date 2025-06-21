#include "pch.hpp"

#include "Engine/Filesystem/File/TTFFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace ProjectA {
	// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6glyf.html
	static void parseCoordinates(ByteStream& stream, sint16* coordinates, uint8* flags, uint16 coordinateCount, uint8 shortVectorBitIndex, uint8 sameOrPositiveVectorBitIndex, FWord minBounds, FWord maxBounds) {
		uint16 coordinateIndex = 0;
		sint16 previousCoordinate = 0;

		while (coordinateIndex < coordinateCount) {
			const uint8 flag = flags[coordinateIndex];
			sint16 delta = 0;

			// If set, the corresponding coordinate is 1 byte long.
			if (flag & BIT(shortVectorBitIndex)) {
				// If the Short Vector bit is set, this bit describes the sign of the value,
				// with a value of 1 equalling positive and a zero value negative.
				const sint8 sign = (flag >> sameOrPositiveVectorBitIndex & BIT(0)) * 2 - 1;

				delta = stream.read<uint8>() * sign;
			}

			// Otherwise, the corresponding coordinate is 2 bytes long.
			else {
				if (~flag & BIT(sameOrPositiveVectorBitIndex)) {
					// If the Short Vector bit is not set, and this bit is not set,
					// the current coordinate is a signed 16-bit delta vector.
					delta = stream.read<sint16>(true);
				}
			}

			const sint16 coordinate = previousCoordinate + delta;

			coordinates[coordinateIndex] = coordinate;
			previousCoordinate = coordinate;

			coordinateIndex += 1;
		}
	}

	static void transformPoint(FWord& x, FWord& y, const GlyphTransform& T) {
		const FWord previousX = x;

		x = static_cast<FWord>(T.m * (x * (T.a / T.m) + y * (T.c / T.m) + T.e));
		y = static_cast<FWord>(T.n * (previousX * (T.b / T.n) + y * (T.d / T.n) + T.f));
	}

	static void parseGlyph(ByteStream& stream, uint32 tableOffset, const sequence<uint16>& glyphOffsets, sequence<Glyph>& glyphs, uint16 glyphIndex, uint16 maxComponentDepth) {
		Glyph& glyph = glyphs[glyphIndex];

		if (!glyph.emptyOrUnparsed()) {
			return;
		}

		stream.seek(tableOffset + glyphOffsets[glyphIndex]);

		const sint16 numberOfContours = stream.read<sint16>(true);

		glyph.min.x = stream.read<FWord>(true);
		glyph.min.y = stream.read<FWord>(true);
		glyph.max.x = stream.read<FWord>(true);
		glyph.max.y = stream.read<FWord>(true);

		if (numberOfContours >= 0) {
			// Parse simple glyph.

			// Array of last points of each contour; n is the number of contours; array entries are point indices.
			sequence<uint16> endPtsOfContours(numberOfContours);

			for (uint16& endPtOfContour : endPtsOfContours) {
				endPtOfContour = stream.read<uint16>(true);
			}

			// Total number of bytes needed for instructions.
			const uint16 instructionLength = stream.read<uint16>(true);

			// Ignore instructions for the moment.
			stream.jump(instructionLength);

			const uint16 numPoints = endPtsOfContours[numberOfContours - 1] + 1;

			// Array of flags.
			sequence<uint8> flags(numPoints);

			{
				uint16 flagIndex = 0;

				while (flagIndex < numPoints) {
					const uint8 flag = stream.read<uint8>();

					uint8 repeatCount = 1;

					if (flag & BIT(3)) {
						// If set, the next byte specifies the number of additional times this set of flags is to be repeated.
						// In this way, the number of flags listed can be smaller than the number of points in a character.
						repeatCount += stream.read<uint8>();

						ENGINE_ASSERT(repeatCount > 0, "The additional repeat count for flag index [] is 0.", flagIndex);
					}

					std::fill_n(&flags[flagIndex], repeatCount, flag);

					flagIndex += repeatCount;
				}

				ENGINE_ASSERT(flagIndex == numPoints, "The flag count is overflowing.");
			}

			// Array of x-coordinates; the first is relative to (0,0), others are relative to previous point.
			sequence<sint16> xCoordinates(numPoints);

			parseCoordinates(stream, &xCoordinates[0], &flags[0], numPoints, 1, 4, glyph.min.x, glyph.max.x);

			// Array of y-coordinates; the first is relative to (0,0), others are relative to previous point.
			sequence<sint16> yCoordinates(numPoints);

			parseCoordinates(stream, &yCoordinates[0], &flags[0], numPoints, 2, 5, glyph.min.y, glyph.max.y);

			sequence<std::vector<ContourPoint>> contours(numberOfContours);

			{
				uint16 contourIndex = 0;
				uint16 startIndex = 0;

				while (contourIndex < numberOfContours) {
					std::vector<ContourPoint>& contour = contours[contourIndex];

					const uint16 endIndex = endPtsOfContours[contourIndex] + 1;
					const bool startOnCurve = flags[startIndex] & BIT(0);

					if (startOnCurve) {
						// Add the start (on-curve) point.
						ContourPoint startPoint{};
						startPoint.x = xCoordinates[startIndex];
						startPoint.y = yCoordinates[startIndex];
						startPoint.onCurve = true;

						contour.emplace_back(startPoint);
					}

					{
						uint16 index = startIndex + 1;
						bool previousOnCurve = startOnCurve;

						while (index < endIndex) {
							ContourPoint point{};
							point.x = xCoordinates[index];
							point.y = yCoordinates[index];
							point.onCurve = flags[index] & BIT(0);

							if (!point.onCurve && !previousOnCurve) {
								// If both this point and the previous one are off-curve,
								// insert an on-curve point between them.
								ContourPoint halfwayPoint{};
								halfwayPoint.x = (point.x + xCoordinates[index - 1]) / 2;
								halfwayPoint.y = (point.y + yCoordinates[index - 1]) / 2;
								halfwayPoint.onCurve = true;

								contour.emplace_back(halfwayPoint);
							}

							contour.emplace_back(point);

							index += 1;
							previousOnCurve = point.onCurve;
						}
					}

					if (!startOnCurve) {
						// Add the start (off-curve) point.
						ContourPoint endPoint{};
						endPoint.x = xCoordinates[startIndex];
						endPoint.y = yCoordinates[startIndex];
						endPoint.onCurve = false;

						contour.emplace_back(endPoint);
					}

					// Close contour.
					contour.emplace_back(contour.front());

					contourIndex += 1;
					startIndex = endIndex;
				}
			}

			#ifdef TTF_FIX_INVERTED_CONTOURS
				if (numberOfContours == 1) {
					std::vector<ContourPoint>& contour = contours[0];

					const uint32 count = static_cast<uint32>(contour.size());

					float32 signedAreaDouble = 0.0f;

					for (uint32 i = 0; i < count; i += 1) {
						const uint32 j = (i + 1) % count;

						// TODO: lyah::area(a, b).
						signedAreaDouble += contour[i].x * contour[j].y - contour[j].x * contour[i].y;
					}

					if (signedAreaDouble > 0.0f) {
						ENGINE_WARN("Glyph [] has a single counter-clockwise contour. Reversing the winding order.", glyphIndex);

						std::reverse(contour.begin(), contour.end());
					}
				}
			#endif

			for (const std::vector<ContourPoint>& contour : contours) {
				for (uint16 edgePointIndex = 1; edgePointIndex < contour.size(); edgePointIndex += 1) {
					ContourPoint contourPoint = contour[edgePointIndex];

					if (!contourPoint.onCurve) {
						continue;
					}

					Edge edge{};
					edge.end.x = contourPoint.x;
					edge.end.y = contourPoint.y;
					edge.curved = !contour[edgePointIndex - 1].onCurve;

					if (edge.curved) {
						edge.start.x = contour[edgePointIndex - 2].x;
						edge.start.y = contour[edgePointIndex - 2].y;
						edge.control.x = contour[edgePointIndex - 1].x;
						edge.control.y = contour[edgePointIndex - 1].y;
					}
					else {
						edge.start.x = contour[edgePointIndex - 1].x;
						edge.start.y = contour[edgePointIndex - 1].y;
					}

					glyph.edges.emplace_back(edge);
				}
			}
		}
		else {
			// Parse compound glyph.
			for (uint16 componentIndex = 0; componentIndex < maxComponentDepth; componentIndex += 1) {
				// Component flag.
				const uint16 flags = stream.read<uint16>(true);

				// Glyph index of component.
				const uint16 componentGlyphIndex = stream.read<uint16>(true);

				GlyphTransform T;

				if (flags & BIT(0)) {
					// If set, arguments are 16-bit, otherwise, they are bytes (uint8 or int8).
					T.e = stream.read<sint16>(true);
					T.f = stream.read<sint16>(true);
				}
				else {
					T.e = stream.read<sint8>();
					T.f = stream.read<sint8>();
				}

				if (flags & BIT(3)) {
					// WE_HAVE_A_SCALE
					T.a = stream.read<sint16>(true);
				}
				else if (flags & BIT(6)) {
					// WE_HAVE_AN_X_AND_Y_SCALE
					T.a = stream.read<sint16>(true);
					T.d = stream.read<sint16>(true);
				}
				else if (flags & BIT(7)) {
					// WE_HAVE_A_TWO_BY_TWO
					T.a = stream.read<sint16>(true);
					T.b = stream.read<sint16>(true);
					T.c = stream.read<sint16>(true);
					T.d = stream.read<sint16>(true);
				}

				// TODO: Implement lyah::max.
				T.m = std::max(abs(T.a), abs(T.b));

				static constexpr float32 G = 33.0f / 65536.0f;

				if (abs(abs(T.a) - abs(T.c)) <= G) {
					T.m *= 2.0f;
				}

				// TODO: Implement lyah::max.
				T.n = std::max(abs(T.c), abs(T.d));

				if (abs(abs(T.b) - abs(T.d)) <= G) {
					T.n *= 2.0f;
				}

				const uint64 offset = stream.getByteOffset();

				if (glyphs[componentGlyphIndex].emptyOrUnparsed()) {
					stream.seek(tableOffset + glyphOffsets[componentGlyphIndex]);

					parseGlyph(stream, tableOffset, glyphOffsets, glyphs, componentGlyphIndex, maxComponentDepth);

					// Reset stream offset after parsing each component glyph.
					stream.seek(offset);
				}

				Glyph componentGlyph = glyphs[componentGlyphIndex];

				for (Edge& edge : componentGlyph.edges) {
					transformPoint(edge.start.x, edge.start.y, T);
					transformPoint(edge.end.x, edge.end.y, T);
					transformPoint(edge.control.x, edge.control.y, T);

					glyph.edges.emplace_back(edge);
				}

				if (!(flags & BIT(5))) {
					break;
				}
			}
		}
	}

	/* #ifdef TTF_PARSER_VERIFY_CHECKSUM
		static uint32 calculateTableChecksum(const uint32* table, uint32 byteCount) {
			uint32 checksum = 0;
			uint32 longCount = (byteCount + 3) / 4;

			// TODO: Refactor.
			while (longCount-- > 0) checksum += *table++;

			return checksum;
		}
	#endif */

	// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html
	template<>
	TTFFile Filesystem::openTemporary(std::string_view path) {
		const File baseFile = openTemporary<File>(path);

		TTFFile file;

		ByteStream stream(baseFile.rawData, baseFile.rawData.count);

		OffsetSubtable offsetSubtable{};
		offsetSubtable.scalerType = stream.read<uint32>(true);

		// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html#ScalerTypeNote
		switch (offsetSubtable.scalerType) {
			case STRING_TO_BIG_ENDIAN_UINT32("true"):
			case 0x00010000:
				break;
			case STRING_TO_BIG_ENDIAN_UINT32("typ1"):
				ENGINE_ASSERT(false, "sfnt-housed PostScript fonts are not supported.");

				return file;
			case STRING_TO_BIG_ENDIAN_UINT32("OTTO"):
				ENGINE_ASSERT(false, "OpenType fonts with PostScript outlines are not supported.");

				return file;
			default:
				ENGINE_ASSERT(false, "Unsupported scaler type. See https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html#ScalerTypeNote.");

				return file;
		}

		offsetSubtable.numTables = stream.read<uint16>(true);
		offsetSubtable.searchRange = stream.read<uint16>(true);
		offsetSubtable.entrySelector = stream.read<uint16>(true);
		offsetSubtable.rangeShift = stream.read<uint16>(true);

		std::unordered_map<uint32, uint32> tableOffsetMap;

		for (uint16 tableIndex = 0; tableIndex < offsetSubtable.numTables; tableIndex += 1) {
			TableDirectory tableDirectory{};
			tableDirectory.tag = stream.read<uint32>();

			#ifdef TTF_PARSER_VERIFY_CHECKSUM
				// TODO: Implement this macro.
				// const uint32 readChecksum = stream.read<uint32>(true);
			#else
				stream.jump(sizeof(uint32));
			#endif

			tableDirectory.offset = stream.read<uint32>(true);
			tableDirectory.length = stream.read<uint32>(true);

			tableOffsetMap.insert(std::make_pair(tableDirectory.tag, tableDirectory.offset));

			/* #ifdef TTF_PARSER_VERIFY_CHECKSUM
				const uint32* start = static_cast<const uint32*>(static_cast<const void*>(stream.getBytePosition() - stream.getByteOffset()));
				const uint32* position = start + tableDirectory.offset;
				const uint32 calculatedChecksum = calculateTableChecksum(position, tableDirectory.length);

				if (calculatedChecksum != readChecksum) {
					const char* tag = static_cast<char*>(static_cast<void*>(&tableDirectory.tag));

					ENGINE_ASSERT(false, "Invalid checksum for TTF table \"[][][][]\".", tag[0], tag[1], tag[2], tag[3]);
				}
			#endif */
		}

		// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6head.html
		auto tableOffset = tableOffsetMap.find(STRING_TO_LITTLE_ENDIAN_UINT32("head"));

		ENGINE_ASSERT(tableOffset != tableOffsetMap.end(), "Could not find 'head' table.");

		stream.seek(tableOffset->second + 50);

		// 0 for short offsets, 1 for long.
		const sint16 indexToLocFormat = stream.read<sint16>(true);

		ENGINE_ASSERT(indexToLocFormat == 0, "IndexToLocFormat = [] is not supported.", indexToLocFormat);

		// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6maxp.html
		tableOffset = tableOffsetMap.find(STRING_TO_LITTLE_ENDIAN_UINT32("maxp"));

		ENGINE_ASSERT(tableOffset != tableOffsetMap.end(), "Could not find 'maxp' table.");

		stream.seek(tableOffset->second + 4);

		const uint16 numGlyphs = stream.read<uint16>(true);

		stream.jump(24);

		const uint16 maxComponentDepth = stream.read<uint16>(true);

		// The actual local offset divided by 2 is stored. The value of n is the number of glyphs in the font + 1.
		// The number of glyphs in the font is found in the maximum profile table.
		sequence<uint16> glyphOffsets(numGlyphs + 1);
		sequence<uint16> glyphLengths(numGlyphs + 1);

		// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6loca.html
		tableOffset = tableOffsetMap.find(STRING_TO_LITTLE_ENDIAN_UINT32("loca"));

		ENGINE_ASSERT(tableOffset != tableOffsetMap.end(), "Could not find 'loca' table.");

		stream.seek(tableOffset->second);

		for (uint16& glyphOffset : glyphOffsets) {
			glyphOffset = stream.read<uint16>(true) * 2;
		}

		const uint16 regCount = static_cast<uint16>(glyphLengths.count / 8);

		for (uint16 regIndex = 0; regIndex < regCount; regIndex += 1) {
			const __m128i next = _mm_load_si128(pointer_cast_const<const __m128i*>(&glyphOffsets[regIndex * 8 + 1]));
			const __m128i curr = _mm_load_si128(pointer_cast_const<const __m128i*>(&glyphOffsets[regIndex * 8]));
			const __m128i sub = _mm_sub_epi16(next, curr);

			_mm_store_si128(pointer_cast<__m128i*>(&glyphLengths[regIndex * 8]), sub);
		}

		// Leave last glyph length as 0.
		for (uint16 glyphIndex = regCount * 8; glyphIndex < numGlyphs; glyphIndex += 1) {
			glyphLengths[glyphIndex] = glyphOffsets[glyphIndex + 1] - glyphOffsets[glyphIndex];
		}

		file.glyphs = sequence<Glyph>(numGlyphs);

		// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6glyf.html
		tableOffset = tableOffsetMap.find(STRING_TO_LITTLE_ENDIAN_UINT32("glyf"));

		ENGINE_ASSERT(tableOffset != tableOffsetMap.end(), "Could not find 'glyf' table.");

		for (uint16 glyphIndex = 0; glyphIndex < file.glyphs.count; glyphIndex += 1) {
			if (glyphLengths[glyphIndex] == 0) {
				// Glyph has no outlines.
				// Since compound glyphs have no reason to reference "empty" glyphs, this check can be omitted within the depth of a compound glyph.
				continue;
			}

			if (!file.glyphs[glyphIndex].emptyOrUnparsed()) {
				// Glyph has already been parsed.
				continue;
			}

			parseGlyph(stream, tableOffset->second, glyphOffsets, file.glyphs, glyphIndex, maxComponentDepth);
		}

		// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6hhea.html
		tableOffset = tableOffsetMap.find(STRING_TO_LITTLE_ENDIAN_UINT32("hhea"));

		ENGINE_ASSERT(tableOffset != tableOffsetMap.end(), "Could not find 'hhea' table.");

		stream.seek(tableOffset->second + 4);

		file.ascent = stream.read<FWord>(true);
		file.descent = stream.read<FWord>(true);
		file.lineGap = stream.read<FWord>(true);

		stream.jump(24);

		const uint16 numOfLongHorMetrics = stream.read<uint16>(true);

		// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6hmtx.html
		tableOffset = tableOffsetMap.find(STRING_TO_LITTLE_ENDIAN_UINT32("hmtx"));

		ENGINE_ASSERT(tableOffset != tableOffsetMap.end(), "Could not find 'hmtx' table.");

		stream.seek(tableOffset->second);

		file.hMetrics = sequence<longHorMetric>(numOfLongHorMetrics);

		for (longHorMetric& hMetric : file.hMetrics) {
			hMetric.advanceWidth = stream.read<uint16>(true);
			hMetric.leftSideBearing = stream.read<sint16>(true);
		}

		file.leftSideBearings = sequence<FWord>(numGlyphs - numOfLongHorMetrics);

		for (FWord& leftSideBearing : file.leftSideBearings) {
			leftSideBearing = stream.read<FWord>(true);
		}

		// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6cmap.html
		tableOffset = tableOffsetMap.find(STRING_TO_LITTLE_ENDIAN_UINT32("cmap"));

		ENGINE_ASSERT(tableOffset != tableOffsetMap.end(), "Could not find 'cmap' table.");

		stream.seek(tableOffset->second + 2);

		// Number of encoding subtables.
		const uint16 numberSubtables = stream.read<uint16>(true);

		uint64 subtableOffset = stream.getByteOffset();
		uint64 unicodeTableOffset = 0;

		// Find Unicode table.
		for (uint16 subtableIndex = 0; subtableIndex < numberSubtables; subtableIndex += 1) {
			stream.seek(subtableOffset);

			// Platform identifier.
			const uint16 platformID = stream.read<uint16>(true);

			// Skip platformSpecificID.
			stream.jump(sizeof(uint16));

			const uint32 offset = stream.read<uint32>(true);

			if (platformID == 0) {
				subtableOffset = stream.getByteOffset();

				stream.seek(tableOffset->second + offset);

				const uint16 format = stream.read<uint16>(true);

				if (format == 4) {
					unicodeTableOffset = stream.getByteOffset();

					break;
				}
			}

			ENGINE_WARN("The Unicode table was not found on the first iteration.");
		}

		ENGINE_ASSERT(unicodeTableOffset > 0, "Failed to find Unicode table.");

		// Parse Unicode table.
		{
			stream.seek(unicodeTableOffset);

			const uint16 length = stream.read<uint16>(true);
			const uint16 language = stream.read<uint16>(true);
			const uint16 segCountX2 = stream.read<uint16>(true);
			const uint16 segCount = segCountX2 / 2;
			const uint16 searchRange = stream.read<uint16>(true);
			const uint16 entrySelector = stream.read<uint16>(true);
			const uint16 rangeShift = stream.read<uint16>(true);

			sequence<uint16> endCodes(segCount);

			for (uint16& endCode : endCodes) {
				endCode = stream.read<uint16>(true);
			}

			stream.jump(sizeof(uint16));

			sequence<uint16> startCodes(segCount);

			for (uint16& startCode : startCodes) {
				startCode = stream.read<uint16>(true);
			}

			sequence<uint16> idDeltas(segCount);

			for (uint16& idDelta : idDeltas) {
				idDelta = stream.read<uint16>(true);
			}

			std::vector<uint16> idRangeOffsets(segCount);

			for (uint16& idRangeOffset : idRangeOffsets) {
				idRangeOffset = stream.read<uint16>(true);
			}

			const uint64 glyphIndexArrayCount = (length - (stream.getByteOffset() - unicodeTableOffset)) / 2;

			idRangeOffsets.resize(segCount + glyphIndexArrayCount);

			for (uint32 glyphIndexIndex = 0; glyphIndexIndex < glyphIndexArrayCount; glyphIndexIndex += 1) {
				idRangeOffsets[segCount + glyphIndexIndex] = stream.read<uint16>(true);
			}

			for (uint16 segmentIndex = 0; segmentIndex < segCount; segmentIndex += 1) {
				const uint16 startCode = startCodes[segmentIndex];
				const uint16 endCode = endCodes[segmentIndex];
				const uint16 idRangeOffset = idRangeOffsets[segmentIndex];

				if (idRangeOffset == 0) {
					for (uint16 code = startCode; code <= endCode; code += 1) {
						const uint32 glyphIndex = static_cast<uint32>(idDeltas[segmentIndex] + code) % 65536;

						file.unicodeMap.insert(std::make_pair(code, glyphIndex));

						if (startCode == endCode) {
							break;
						}
					}
				}
				else {
					// If the idRangeOffset value for the segment is not 0,
					// the mapping of the character codes relies on the glyphIndexArray.
					for (uint16 code = startCode; code <= endCode; code += 1) {
						const uint32 glyphIndex = *(&idRangeOffsets[segmentIndex] + (idRangeOffsets[segmentIndex] / 2) + (code - startCode));

						file.unicodeMap.insert(std::make_pair(code, glyphIndex));

						if (startCode == endCode) {
							break;
						}
					}
				}
			}
		}

		return file;
	}
}