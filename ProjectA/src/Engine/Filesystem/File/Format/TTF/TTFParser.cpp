#include "pch.hpp"

#include "Engine/Filesystem/File/TTFFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace ProjectA {
	/* #ifdef TTF_PARSER_VERIFY_CHECKSUM
		static uint32 calculateTableChecksum(const uint32* table, uint32 byteCount) {
			uint32 checksum = 0;
			uint32 longCount = (byteCount + 3) / 4;

			// TODO: Refactor.
			while (longCount-- > 0) checksum += *table++;

			return checksum;
		}
	#endif */

	// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6head.html
	static sint16 parseHeadTable(ByteStream& stream) {
		Fixed version{};
		version.integer = stream.read<uint16>(true);
		version.fraction = stream.read<uint16>(true);

		Fixed fontRevision{};
		fontRevision.integer = stream.read<uint16>(true);
		fontRevision.fraction = stream.read<uint16>(true);

		const uint32 checkSumAdjustment = stream.read<uint32>(true);

		const uint32 magicNumber = stream.read<uint32>(true);

		const uint16 flags = stream.read<uint16>(true);
		const uint16 unitsPerEm = stream.read<uint16>(true);

		const longDateTime created = stream.read<longDateTime>(true);
		const longDateTime modified = stream.read<longDateTime>(true);

		const FWord xMin = stream.read<FWord>(true);
		const FWord yMin = stream.read<FWord>(true);
		const FWord xMax = stream.read<FWord>(true);
		const FWord yMax = stream.read<FWord>(true);

		const uint16 macStyle = stream.read<uint16>(true);
		const uint16 lowestRecPPEM = stream.read<uint16>(true);
		const sint16 fontDirectionHint = stream.read<sint16>(true);

		// 0 for short offsets, 1 for long.
		const sint16 indexToLocFormat = stream.read<sint16>(true);

		ENGINE_ASSERT(indexToLocFormat == 0, "IndexToLocFormat = [] is not supported.", indexToLocFormat);

		const sint16 glyphDataFormat = stream.read<sint16>(true);

		return indexToLocFormat;
	}

	// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6maxp.html
	static maxp parseMaxpTable(ByteStream& stream) {
		maxp maxp{};
		maxp.version.integer = stream.read<uint16>(true);
		maxp.version.fraction = stream.read<uint16>(true);
		maxp.numGlyphs = stream.read<uint16>(true);
		maxp.maxPoints = stream.read<uint16>(true);
		maxp.maxContours = stream.read<uint16>(true);
		maxp.maxComponentPoints = stream.read<uint16>(true);
		maxp.maxComponentContours = stream.read<uint16>(true);
		maxp.maxZones = stream.read<uint16>(true);
		maxp.maxTwilightPoints = stream.read<uint16>(true);
		maxp.maxStorage = stream.read<uint16>(true);
		maxp.maxFunctionDefs = stream.read<uint16>(true);
		maxp.maxInstructionDefs = stream.read<uint16>(true);
		maxp.maxStackElements = stream.read<uint16>(true);
		maxp.maxSizeOfInstructions = stream.read<uint16>(true);
		maxp.maxComponentElements = stream.read<uint16>(true);
		maxp.maxComponentDepth = stream.read<uint16>(true);

		return maxp;
	}

	// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6hhea.html
	static hhea parseHheaTable(ByteStream& stream) {
		hhea hhea{};
		hhea.version.integer = stream.read<uint16>(true);
		hhea.version.fraction = stream.read<uint16>(true);
		hhea.ascent = stream.read<FWord>(true);
		hhea.descent = stream.read<FWord>(true);
		hhea.lineGap = stream.read<FWord>(true);
		hhea.advanceWidthMax = stream.read<uFWord>(true);
		hhea.minLeftSideBearing = stream.read<FWord>(true);
		hhea.minRightSideBearing = stream.read<FWord>(true);
		hhea.xMaxExtent = stream.read<FWord>(true);
		hhea.caretSlopeRise = stream.read<sint16>(true);
		hhea.caretSlopeRun = stream.read<sint16>(true);
		hhea.caretOffset = stream.read<FWord>(true);

		stream.jump(4 * sizeof(sint16));

		hhea.metricDataFormat = stream.read<sint16>(true);
		hhea.numOfLongHorMetrics = stream.read<uint16>(true);

		return hhea;
	}

	static void transformPoint(FWord& x, FWord& y, const GlyphTransform& T) {
		const FWord previousX = x;

		x = static_cast<FWord>(T.m * (x * (T.a / T.m) + y * (T.c / T.m) + T.e));
		y = static_cast<FWord>(T.n * (previousX * (T.b / T.n) + y * (T.d / T.n) + T.f));
	}

	static void transformGlyph(Glyph& glyph, const GlyphTransform& T) {
		transformPoint(glyph.max.x, glyph.max.y, T);
		transformPoint(glyph.min.x, glyph.min.y, T);

		for (Edge& edge : glyph.edges) {
			transformPoint(edge.start.x, edge.start.y, T);
			transformPoint(edge.end.x, edge.end.y, T);
			transformPoint(edge.control.x, edge.control.y, T);
		}
	}

	// See https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6glyf.html.
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

	static Glyph parseCompoundGlyph(ByteStream& stream, std::vector<Glyph>& glyphs, uint32 tableOffset, const sequence<uint16>& glyphOffsets, uint16 compoundGlyphIndex, uint16 maxComponentDepth);

	static Glyph parseSimpleGlyph(ByteStream& stream, std::vector<Glyph>& glyphs, uint32 tableOffset, const sequence<uint16>& glyphOffsets, uint16 glyphIndex, uint16 maxComponentDepth) {
		stream.reset();
		stream.jump(static_cast<uint64>(tableOffset + glyphOffsets[glyphIndex]));

		// If the number of contours is positive or zero, it is a single glyph;
		// If the number of contours less than zero, the glyph is compound.
		const sint16 numberOfContours = stream.read<sint16>(true);

		if (numberOfContours < 0) {
			return parseCompoundGlyph(stream, glyphs, tableOffset, glyphOffsets, glyphIndex, maxComponentDepth);
		}

		Glyph glyph;
		glyph.min.x = stream.read<FWord>(true);
		glyph.min.y = stream.read<FWord>(true);
		glyph.max.x = stream.read<FWord>(true);
		glyph.max.y = stream.read<FWord>(true);

		// Array of last points of each contour; n is the number of contours; array entries are point indices.
		sequence<uint16> endPtsOfContours(numberOfContours);

		for (uint16& endPtOfContour : endPtsOfContours) {
			endPtOfContour = stream.read<uint16>(true);
		}

		// Total number of bytes needed for instructions.
		const uint16 instructionLength = stream.read<uint16>(true);

		stream.jump(instructionLength);

		const uint16 numPoints = endPtsOfContours[numberOfContours - 1] + 1;

		// Array of flags.
		std::vector<uint8> flags(numPoints);

		{
			uint16 flagIndex = 0;

			while (flagIndex < numPoints) {
				const uint8 flag = stream.read<uint8>();

				uint8 repeatCount = 1;

				if (flag & BIT(3)) {
					// If set, the next byte specifies the number of additional times this set of flags is to be repeated.
					// In this way, the number of flags listed can be smaller than the number of points in a character.
					repeatCount += stream.read<uint8>();

					ENGINE_ASSERT(repeatCount > 1, "The additional repeat count for flag index [] is 0.", flagIndex);
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
					ENGINE_WARN("Glyph [] has a single counter-clockwise contour. Inverting the winding order.", glyphIndex);

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

		return glyph;
	}

	static Glyph parseCompoundGlyph(ByteStream& stream, std::vector<Glyph>& glyphs, uint32 tableOffset, const sequence<uint16>& glyphOffsets, uint16 compoundGlyphIndex, uint16 maxComponentDepth) {
		stream.reset();
		stream.jump(static_cast<uint64>(tableOffset + glyphOffsets[compoundGlyphIndex]));

		// Skip number of contours.
		stream.jump(sizeof(sint16));

		Glyph glyph;
		glyph.min.x = stream.read<FWord>(true);
		glyph.min.y = stream.read<FWord>(true);
		glyph.max.x = stream.read<FWord>(true);
		glyph.max.y = stream.read<FWord>(true);

		for (uint16 depth = 0; depth < maxComponentDepth; depth += 1) {
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

			// TODO: lyah::max(float32, float32)
			T.m = std::max(abs(T.a), abs(T.b));

			constexpr float32 value = 33.0f / 65536.0f;

			if (abs(abs(T.a) - abs(T.c)) <= value) {
				T.m *= 2.0f;
			}

			T.n = std::max(abs(T.c), abs(T.d));

			if (abs(abs(T.b) - abs(T.d)) <= value) {
				T.n *= 2.0f;
			}

			const uint64 byteOffset = stream.getByteOffset();

			if (glyphs[componentGlyphIndex].edges.empty()) {
				Glyph componentGlyph = parseSimpleGlyph(stream, glyphs, tableOffset, glyphOffsets, componentGlyphIndex, maxComponentDepth);

				glyphs[componentGlyphIndex] = componentGlyph;

				transformGlyph(componentGlyph, T);

				for (const Edge& edge : componentGlyph.edges) {
					glyph.edges.emplace_back(edge);
				}

				// Reset stream after parsing component glyph.
				stream.reset();
				stream.jump(byteOffset);
			}
			else {
				const Glyph& componentGlyph = glyphs[componentGlyphIndex];

				for (const Edge& edge : componentGlyph.edges) {
					glyph.edges.emplace_back(edge);
				}
			}

			if (!(flags & BIT(5))) {
				break;
			}
		}

		return glyph;
	}

	// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html
	template<>
	TTFFile Filesystem::openTemporary(std::string_view path) {
		const File baseFile = openTemporary<File>(path);

		TTFFile file;

		ByteStream stream(baseFile.rawData, baseFile.rawData.count);

		// Read offset subtable.
		OffsetSubtable offsetSubtable{};
		offsetSubtable.scalerType = stream.read<uint32>(true);

		switch (offsetSubtable.scalerType) {
			case STRING_TO_BIG_ENDIAN_UINT32("true"):
			case 0x00010000:
				break;
			case STRING_TO_BIG_ENDIAN_UINT32("typ1"):
				ENGINE_ASSERT(false, "sfnt-housed PostScript fonts are not supported.");

				break;
			case STRING_TO_BIG_ENDIAN_UINT32("OTTO"):
				ENGINE_ASSERT(false, "OpenType fonts with PostScript outlines are not supported.");

				break;
			default:
				ENGINE_ASSERT(false, "Unsupported scaler type. See https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html#ScalerTypeNote.");
		}

		offsetSubtable.numTables = stream.read<uint16>(true);
		offsetSubtable.searchRange = stream.read<uint16>(true);
		offsetSubtable.entrySelector = stream.read<uint16>(true);
		offsetSubtable.rangeShift = stream.read<uint16>(true);

		std::unordered_map<uint32, uint32> tableOffsetMap;

		for (uint32 tableIndex = 0; tableIndex < offsetSubtable.numTables; tableIndex += 1) {
			TableDirectory tableDirectory{};
			tableDirectory.tag = stream.read<uint32>();

			stream.jump(sizeof(uint32));
			/*#ifdef TTF_PARSER_VERIFY_CHECKSUM
				//const uint32 readChecksum = stream.read<uint32>(true);
			#else
				stream.jump(sizeof(uint32));
			#endif*/

			tableDirectory.offset = stream.read<uint32>(true);
			tableDirectory.length = stream.read<uint32>(true);

			tableOffsetMap.insert(std::make_pair(tableDirectory.tag, tableDirectory.offset));

			// TODO: Handle table checksum.
			/*#ifdef TTF_PARSER_VERIFY_CHECKSUM
				const uint32* start = static_cast<const uint32*>(static_cast<const void*>(stream.getBytePosition() - stream.getByteOffset()));
				const uint32* position = start + tableDirectory.offset;
				const uint32 calculatedChecksum = calculateTableChecksum(position, tableDirectory.length);

				if (calculatedChecksum != readChecksum) {
					const char* tag = static_cast<char*>(static_cast<void*>(&tableDirectory.tag));

					ENGINE_ASSERT(false, "Invalid checksum for TTF table \"[][][][]\".", tag[0], tag[1], tag[2], tag[3]);
				}
			#endif*/
		}

		// Parse 'head' table.
		auto tableOffset = tableOffsetMap.find(STRING_TO_LITTLE_ENDIAN_UINT32("head"));

		ENGINE_ASSERT(tableOffset != tableOffsetMap.end(), "Could not find 'head' table.");

		stream.reset();
		stream.jump(tableOffset->second);

		const sint16 indexToLocFormat = parseHeadTable(stream);

		// Parse 'maxp' table.
		tableOffset = tableOffsetMap.find(STRING_TO_LITTLE_ENDIAN_UINT32("maxp"));

		ENGINE_ASSERT(tableOffset != tableOffsetMap.end(), "Could not find 'maxp' table.");

		stream.reset();
		stream.jump(static_cast<uint64>(tableOffset->second));

		file.maxp = parseMaxpTable(stream);

		// The actual local offset divided by 2 is stored. The value of n is the number of glyphs in the font + 1.
		// The number of glyphs in the font is found in the maximum profile table.
		sequence<uint16> glyphOffsets(file.maxp.numGlyphs + 1);

		sequence<uint16> glyphLengths(file.maxp.numGlyphs + 1);

		// Parse 'loca' table.
		// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6loca.html
		tableOffset = tableOffsetMap.find(STRING_TO_LITTLE_ENDIAN_UINT32("loca"));

		ENGINE_ASSERT(tableOffset != tableOffsetMap.end(), "Could not find 'loca' table.");

		stream.reset();
		stream.jump(static_cast<uint64>(tableOffset->second));

		for (uint16& glyphOffset : glyphOffsets) {
			glyphOffset = stream.read<uint16>(true) * 2;
		}

		const uint16 regCount = static_cast<uint16>(glyphLengths.count / 8);

		for (uint16 regIndex = 0; regIndex < regCount; regIndex += 1) {
			const __m128i next = _mm_load_si128(static_cast<__m128i*>(static_cast<void*>(&glyphOffsets[regIndex * 8 + 1])));
			const __m128i curr = _mm_load_si128(static_cast<__m128i*>(static_cast<void*>(&glyphOffsets[regIndex * 8])));
			const __m128i sub = _mm_sub_epi16(next, curr);

			_mm_store_si128(static_cast<__m128i*>(static_cast<void*>(&glyphLengths[regIndex * 8])), sub);
		}

		// Leave last glyph length as 0.
		for (uint16 glyphIndex = regCount * 8; glyphIndex < file.maxp.numGlyphs; glyphIndex += 1) {
			glyphLengths[glyphIndex] = glyphOffsets[glyphIndex + 1] - glyphOffsets[glyphIndex];
		}

		file.glyphs.resize(file.maxp.numGlyphs);

		// Parse 'glyf' table.
		// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6glyf.html
		tableOffset = tableOffsetMap.find(STRING_TO_LITTLE_ENDIAN_UINT32("glyf"));

		ENGINE_ASSERT(tableOffset != tableOffsetMap.end(), "Could not find 'glyf' table.");

		for (uint16 glyphIndex = 0; glyphIndex < file.maxp.numGlyphs; glyphIndex += 1) {
			// Need to reset + jump to peek the number of contours.
			stream.reset();
			stream.jump(static_cast<uint64>(tableOffset->second + glyphOffsets[glyphIndex]));

			// If the number of contours is positive or zero, it is a single glyph;
			// If the number of contours less than zero, the glyph is compound.
			const sint16 numberOfContours = stream.peek<sint16>(true);

			if (glyphLengths[glyphIndex] == 0) {
				continue;
			}

			if (!file.glyphs[glyphIndex].edges.empty()) {
				// This glyph has already been parsed.
				continue;
			}

			if (numberOfContours >= 0) {
				file.glyphs[glyphIndex] = parseSimpleGlyph(stream, file.glyphs, tableOffset->second, glyphOffsets, glyphIndex, file.maxp.maxComponentDepth);
			}
			else {
				file.glyphs[glyphIndex] = parseCompoundGlyph(stream, file.glyphs, tableOffset->second, glyphOffsets, glyphIndex, file.maxp.maxComponentDepth);
			}
		}

		// Parse 'hhea' table.
		tableOffset = tableOffsetMap.find(STRING_TO_LITTLE_ENDIAN_UINT32("hhea"));

		ENGINE_ASSERT(tableOffset != tableOffsetMap.end(), "Could not find 'hhea' table.");

		stream.reset();
		stream.jump(static_cast<uint64>(tableOffset->second));

		file.hhea = parseHheaTable(stream);

		// Parse 'hmtx' table.
		// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6hmtx.html
		tableOffset = tableOffsetMap.find(STRING_TO_LITTLE_ENDIAN_UINT32("hmtx"));

		ENGINE_ASSERT(tableOffset != tableOffsetMap.end(), "Could not find 'hmtx' table.");

		stream.reset();
		stream.jump(static_cast<uint64>(tableOffset->second));

		file.hMetrics = sequence<longHorMetric>(file.hhea.numOfLongHorMetrics);

		for (longHorMetric& hMetric : file.hMetrics) {
			hMetric.advanceWidth = stream.read<uint16>(true);
			hMetric.leftSideBearing = stream.read<sint16>(true);
		}

		file.leftSideBearings = sequence<FWord>(file.maxp.numGlyphs - file.hhea.numOfLongHorMetrics);

		for (FWord& leftSideBearing : file.leftSideBearings) {
			leftSideBearing = stream.read<FWord>(true);
		}

		// Parse 'cmap' table.
		// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6cmap.html
		tableOffset = tableOffsetMap.find(STRING_TO_LITTLE_ENDIAN_UINT32("cmap"));

		ENGINE_ASSERT(tableOffset != tableOffsetMap.end(), "Could not find 'cmap' table.");

		stream.reset();
		stream.jump(static_cast<uint64>(tableOffset->second));

		// Skip version.
		stream.jump(sizeof(uint16));

		// Number of encoding subtables.
		const uint16 numberSubtables = stream.read<uint16>(true);

		uint64 subtableOffset = stream.getByteOffset();
		uint64 unicodeTableOffset = 0;

		// Find Unicode table.
		for (uint16 subtableIndex = 0; subtableIndex < numberSubtables; subtableIndex += 1) {
			stream.reset();
			stream.jump(subtableOffset);

			// Platform identifier.
			const uint16 platformID = stream.read<uint16>(true);

			// Skip platformSpecificID.
			stream.jump(sizeof(uint16));

			const uint32 offset = stream.read<uint32>(true);

			if (platformID == 0) {
				subtableOffset = stream.getByteOffset();

				stream.reset();
				stream.jump(static_cast<uint64>(tableOffset->second + offset));

				const uint16 format = stream.read<uint16>(true);

				if (format == 4) {
					unicodeTableOffset = stream.getByteOffset();

					break;
				}
			}

			ENGINE_WARN("The Unicode table was not found on the first iteration.");
		}

		ENGINE_ASSERT(unicodeTableOffset != 0, "Failed to find Unicode table.");

		// Parse Unicode table.
		{
			stream.reset();
			stream.jump(unicodeTableOffset);

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

						// TODO: Debug-only.
						if (glyphIndex != 0) {
							file.glyphs[glyphIndex].codePoint = code;
						}

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

						// TODO: Debug-only.
						if (glyphIndex != 0) {
							file.glyphs[glyphIndex].codePoint = code;
						}

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