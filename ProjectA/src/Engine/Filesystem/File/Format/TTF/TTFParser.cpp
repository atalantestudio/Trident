#include "pch.hpp"

#include "Engine/Filesystem/File/TTFFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace ProjectA {
	struct OffsetSubtable {
		// A tag to indicate the OFA scaler to be used to rasterize this font.
		uint32 scalerType;

		// Number of tables.
		uint16 numTables;

		// (maximum power of 2 <= numTables) * 16.
		uint16 searchRange;

		// log2(maximum power of 2 <= numTables).
		uint16 entrySelector;

		// numTables * 16 - searchRange.
		uint16 rangeShift;
	};

	struct TableDirectory {
		// 4-byte identifier.
		uint32 tag;

		// Checksum for this table.
		uint32 checksum;

		// Offset from beginning of sfnt.
		uint32 offset;

		// Length of this table in byte (actual length not padded length).
		uint32 length;
	};

	struct maxp {
		// 0x00010000 (1.0)
		Fixed version;

		// The number of glyphs in the font.
		uint16 numGlyphs;

		// Points in non-compound glyph.
		uint16 maxPoints;

		// Contours in non-compound glyph.
		uint16 maxContours;

		// Points in compound glyph.
		uint16 maxComponentPoints;

		// Contours in compound glyph.
		uint16 maxComponentContours;

		// Set to 2.
		uint16 maxZones;

		//points used in Twilight Zone (Z0)
		uint16 maxTwilightPoints;

		// Number of Storage Area locations.
		uint16 maxStorage;

		// Number of FDEFs.
		uint16 maxFunctionDefs;

		// Number of IDEFs.
		uint16 maxInstructionDefs;

		// Maximum stack depth.
		uint16 maxStackElements;

		// Byte count for glyph instructions.
		uint16 maxSizeOfInstructions;

		// Number of glyphs referenced at top level.
		uint16 maxComponentElements;

		// Levels of recursion, set to 0 if font has only simple glyphs.
		uint16 maxComponentDepth;
	};

	static uint32 calculateTableChecksum(uint32* table, uint32 byteCount) {
		uint32 checksum = 0;
		uint32 longCount = (byteCount + 3) / 4;

		while (longCount > 0) {
			checksum += *table;

			table += 1;
			longCount -= 1;
		}

		return checksum;
	}

	// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6head.html
	static sint16 parseHeadTable(ByteStream& stream) {
		Fixed version;
		version.integer = stream.read<uint16>(true);
		version.fraction = stream.read<uint16>(true);

		Fixed fontRevision;
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

	struct GlyphTransform {
		float32 a = 1.0f;
		float32 b = 0.0f;
		float32 c = 0.0f;
		float32 d = 1.0f;
		float32 e = 0.0f;
		float32 f = 0.0f;
		float32 m = 0.0f;
		float32 n = 0.0f;
	};

	static void transformPoint(FWord& x, FWord& y, const GlyphTransform& T) {
		const FWord xCopy = x;

		x = static_cast<FWord>(T.m * (x * (T.a / T.m) + y * (T.c / T.m) + T.e));
		y = static_cast<FWord>(T.n * (xCopy * (T.b / T.n) + y * (T.d / T.n) + T.f));
	}

	static void transformGlyph(Glyph& glyph, const GlyphTransform& T) {
		for (Edge& edge : glyph.edges) {
			transformPoint(edge.start.x, edge.start.y, T);
			transformPoint(edge.end.x, edge.end.y, T);
			transformPoint(edge.control.x, edge.control.y, T);
		}

		transformPoint(glyph.max.x, glyph.max.y, T);
		transformPoint(glyph.min.x, glyph.min.y, T);
	}

	// See https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6glyf.html.
	static void parseCoordinates(ByteStream& stream, sint16* coordinates, uint8* flags, uint16 coordinateCount, uint8 shortVectorBitIndex, uint8 sameOrPositiveVectorBitIndex, FWord minBounds, FWord maxBounds) {
		uint16 coordinateIndex = 0;
		sint16 previousCoordinate = 0;

		while (coordinateIndex < coordinateCount) {
			const uint8 flag = flags[coordinateIndex];
			sint16 delta = 0;

			if (flag & BIT(shortVectorBitIndex)) {
				// If set, the corresponding coordinate is 1 byte long.

				// If the Short Vector bit is set, this bit describes the sign of the value,
				// with a value of 1 equalling positive and a zero value negative.
				const sint8 sign = (flag >> sameOrPositiveVectorBitIndex & BIT(0)) * 2 - 1;

				delta = stream.read<uint8>() * sign;
			}
			else {
				// Otherwise, the corresponding coordinate is 2 bytes long.

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

	// fwd
	static Glyph parseCompoundGlyph(ByteStream& stream, uint32 tableOffset, const fchain<uint16>& glyphOffsets, uint16 compoundGlyphIndex, uint16 maxComponentDepth);

	static Glyph parseSimpleGlyph(ByteStream& stream, uint32 tableOffset, const fchain<uint16>& glyphOffsets, uint16 glyphIndex, uint16 maxComponentDepth) {
		stream.reset();
		stream.jump(static_cast<uint64>(tableOffset + glyphOffsets[glyphIndex]));

		// If the number of contours is positive or zero, it is a single glyph;
		// If the number of contours less than zero, the glyph is compound.
		const sint16 numberOfContours = stream.read<sint16>(true);

		if (numberOfContours < 0) {
			return parseCompoundGlyph(stream, tableOffset, glyphOffsets, glyphIndex, maxComponentDepth);
		}

		Glyph glyph;

		// Minimum x for coordinate data.
		glyph.min.x = stream.read<FWord>(true);

		// Minimum y for coordinate data.
		glyph.min.y = stream.read<FWord>(true);

		// Maximum x for coordinate data.
		glyph.max.x = stream.read<FWord>(true);

		// Maximum y for coordinate data.
		glyph.max.y = stream.read<FWord>(true);

		// Array of last points of each contour; n is the number of contours; array entries are point indices.
		fchain<uint16> endPtsOfContours(numberOfContours);

		for (uint16& endPtOfContour : endPtsOfContours) {
			endPtOfContour = stream.read<uint16>(true);
		}

		// Total number of bytes needed for instructions.
		const uint16 instructionLength = stream.read<uint16>(true);

		stream.jump(instructionLength);

		const uint16 numPoints = endPtsOfContours[numberOfContours - 1] + 1;

		// Array of flags.
		chain<uint8> flags(numPoints);

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
		fchain<sint16> xCoordinates(numPoints);

		parseCoordinates(stream, &xCoordinates[0], &flags[0], numPoints, 1, 4, glyph.min.x, glyph.max.x);

		// Array of y-coordinates; the first is relative to (0,0), others are relative to previous point.
		fchain<sint16> yCoordinates(numPoints);

		parseCoordinates(stream, &yCoordinates[0], &flags[0], numPoints, 2, 5, glyph.min.y, glyph.max.y);

		fchain<chain<ContourPoint>> contours(numberOfContours);

		{
			uint16 contourIndex = 0;
			uint16 startIndex = 0;

			while (contourIndex < numberOfContours) {
				chain<ContourPoint>& contour = contours[contourIndex];

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

		for (const chain<ContourPoint>& contour : contours) {
			for (uint16 edgePointIndex = 1; edgePointIndex < contour.size(); edgePointIndex += 1) {
				const ContourPoint& contourPoint = contour[edgePointIndex];

				if (!contourPoint.onCurve) {
					continue;
				}

				Edge edge{};
				edge.end.x = contourPoint.x;
				edge.end.y = contourPoint.y;
				edge.isQuadraticCurve = !contour[edgePointIndex - 1].onCurve;

				if (edge.isQuadraticCurve) {
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

	static Glyph parseCompoundGlyph(ByteStream& stream, uint32 tableOffset, const fchain<uint16>& glyphOffsets, uint16 compoundGlyphIndex, uint16 maxComponentDepth) {
		stream.reset();
		stream.jump(static_cast<uint64>(tableOffset + glyphOffsets[compoundGlyphIndex]));

		// Skip number of contours.
		stream.jump(sizeof(sint16));

		Glyph glyph;

		// Minimum x for coordinate data.
		glyph.min.x = stream.read<FWord>(true);

		// Minimum y for coordinate data.
		glyph.min.y = stream.read<FWord>(true);

		// Maximum x for coordinate data.
		glyph.max.x = stream.read<FWord>(true);

		// Maximum y for coordinate data.
		glyph.max.y = stream.read<FWord>(true);

		for (uint16 depth = 0; depth < maxComponentDepth; depth += 1) {
			// Component flag.
			const uint16 flags = stream.read<uint16>(true);

			// Glyph index of component.
			const uint16 glyphIndex = stream.read<uint16>(true);

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
			// TODO: ProjectA::abs(float32, float32)
			T.m = std::max(lyah::abs(T.a), lyah::abs(T.b));

			if (lyah::abs(lyah::abs(T.a) - lyah::abs(T.c)) <= 33.0f / 65536.0f) {
				T.m *= 2.0f;
			}

			T.n = std::max(lyah::abs(T.c), lyah::abs(T.d));

			if (lyah::abs(lyah::abs(T.b) - lyah::abs(T.d)) <= 33.0f / 65536.0f) {
				T.n *= 2.0f;
			}

			const uint64 byteOffset = stream.getByteOffset();

			Glyph componentGlyph = parseSimpleGlyph(stream, tableOffset, glyphOffsets, glyphIndex, maxComponentDepth);

			transformGlyph(componentGlyph, T);

			for (const Edge& edge : componentGlyph.edges) {
				glyph.edges.emplace_back(edge);
			}

			// Reset stream after parsing component glyph.
			stream.reset();
			stream.jump(byteOffset);

			if (!(flags & BIT(5))) {
				break;
			}
		}

		return glyph;
	}

	// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6.html
	template<>
	TTFFile Filesystem::load(const std::string& path) {
		const File file = load<File>(path);

		TTFFile ttfFile;
		ttfFile.name = file.name;
		ttfFile.absolutePath = file.absolutePath;
		ttfFile.size = file.size;
		ttfFile.rawData = file.rawData;

		ByteStream stream(file.rawData.data(), file.rawData.size());

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
			tableDirectory.checksum = stream.read<uint32>(true);
			tableDirectory.offset = stream.read<uint32>(true);
			tableDirectory.length = stream.read<uint32>(true);

			tableOffsetMap.insert(std::make_pair(tableDirectory.tag, tableDirectory.offset));

			const char* TypeString = static_cast<char*>(static_cast<void*>(&tableDirectory.tag));

			ENGINE_INFO("Reading directory for table \"[][][][]\".", TypeString[0], TypeString[1], TypeString[2], TypeString[3]);

			// TODO: Calculate checksum.
			//const uint32 checksum = calculateTableChecksum((uint32*)(stream.getBytePosition() - stream.getByteOffset() + tableDirectory.offset), tableDirectory.length);
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

		const maxp maxp = parseMaxpTable(stream);

		// The actual local offset divided by 2 is stored. The value of n is the number of glyphs in the font + 1.
		// The number of glyphs in the font is found in the maximum profile table.
		fchain<uint16> glyphOffsets(maxp.numGlyphs + 1);

		fchain<uint16> glyphLengths(maxp.numGlyphs + 1);

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
		for (uint16 glyphIndex = regCount * 8; glyphIndex < maxp.numGlyphs; glyphIndex += 1) {
			glyphLengths[glyphIndex] = glyphOffsets[glyphIndex + 1] - glyphOffsets[glyphIndex];
		}

		ttfFile.glyphs.resize(maxp.numGlyphs);

		// Parse 'glyf' table.
		// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6glyf.html
		tableOffset = tableOffsetMap.find(STRING_TO_LITTLE_ENDIAN_UINT32("glyf"));

		ENGINE_ASSERT(tableOffset != tableOffsetMap.end(), "Could not find 'glyf' table.");

		for (uint16 glyphIndex = 0; glyphIndex < maxp.numGlyphs; glyphIndex += 1) {
			// Need to reset + jump to peek the number of contours.
			stream.reset();
			stream.jump(static_cast<uint64>(tableOffset->second + glyphOffsets[glyphIndex]));

			// If the number of contours is positive or zero, it is a single glyph;
			// If the number of contours less than zero, the glyph is compound.
			const sint16 numberOfContours = stream.peek<sint16>(true);

			if (glyphLengths[glyphIndex] == 0) {
				ENGINE_WARN("Number of contours for glyph [] is 0.", glyphIndex);

				continue;
			}

			if (numberOfContours >= 0) {
				ttfFile.glyphs[glyphIndex] = parseSimpleGlyph(stream, tableOffset->second, glyphOffsets, glyphIndex, maxp.maxComponentDepth);
			}
			else {
				ttfFile.glyphs[glyphIndex] = parseCompoundGlyph(stream, tableOffset->second, glyphOffsets, glyphIndex, maxp.maxComponentDepth);
			}
		}

		return ttfFile;
	}
}