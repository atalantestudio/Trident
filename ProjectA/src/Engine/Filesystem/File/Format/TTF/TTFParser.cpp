#include "pch.hpp"

#include "Engine/Filesystem/File/TTFFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"
#include "Engine/Filesystem/Stream/ByteStream.hpp"

namespace ProjectA {
	// TODO: 16-bit signed fraction.

	// 16.16-bit signed fixed-point number.
	struct Fixed {
		uint16 integer;
		uint16 fraction;
	};

	// 16-bit signed integer that describes a quantity in FUnits, the smallest measurable distance in em space.
	typedef sint16 FWord;

	// 16-bit unsigned integer that describes a quantity in FUnits, the smallest measurable distance in em space.
	typedef uint16 uFWord;

	// TODO: 16-bit signed fixed number with the low 14 bits representing fraction.

	// The long internal format of a date in seconds since 12:00 midnight, January 1, 1904.
	// It is represented as a signed 64-bit integer.
	typedef sint64 longDateTime;

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

	struct Point {
		FWord x;
		FWord y;
	};

	struct EdgePoint {
		FWord x;
		FWord y;
		bool onCurve;
	};

	struct Edge {
		EdgePoint start;
		EdgePoint end;
		EdgePoint control;
		bool isQuadraticCurve;
	};

	struct Glyph {
		Point min;
		Point max;
		chain<Edge> edges;
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

		/*const uint16 regCount = static_cast<uint16>(glyphLengths.count / 8);

		for (uint16 regIndex = 0; regIndex < regCount; regIndex += 1) {
			const __m128i next = _mm_load_si128(static_cast<__m128i*>(static_cast<void*>(&glyphOffsets[regIndex * 8 + 1])));
			const __m128i curr = _mm_load_si128(static_cast<__m128i*>(static_cast<void*>(&glyphOffsets[regIndex * 8])));
			const __m128i sub = _mm_sub_epi16(next, curr);

			_mm_store_si128(static_cast<__m128i*>(static_cast<void*>(&glyphLengths[regIndex * 8])), sub);
		}

		// Leave last glyph length as 0.
		for (uint16 glyphIndex = regCount * 8; glyphIndex < maxp.numGlyphs; glyphIndex += 1) {
			glyphLengths[glyphIndex] = glyphOffsets[glyphIndex + 1] - glyphOffsets[glyphIndex];
		}*/

		fchain<Glyph> glyphs(maxp.numGlyphs);

		// Parse 'glyf' table.
		// https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6glyf.html
		tableOffset = tableOffsetMap.find(STRING_TO_LITTLE_ENDIAN_UINT32("glyf"));

		ENGINE_ASSERT(tableOffset != tableOffsetMap.end(), "Could not find 'glyf' table.");

		// 'A'
		const uint16 testGlyphIndex = 4;

		for (uint16 glyphIndex = testGlyphIndex; glyphIndex < glyphs.count; glyphIndex += 1) {
			stream.reset();
			stream.jump(static_cast<uint64>(tableOffset->second));

			const uint16 glyphOffset = glyphOffsets[glyphIndex];

			stream.jump(glyphOffset);

			// If the number of contours is positive or zero, it is a single glyph;
			// If the number of contours less than zero, the glyph is compound.
			const sint16 numberOfContours = stream.read<sint16>(true);

			Glyph glyph;

			if (numberOfContours == 0) {
				glyph.max = {0, 0};
				glyph.max = {0, 0};
			}
			else if (numberOfContours > 0) {
				// Minimum x for coordinate data.
				const FWord xMin = stream.read<FWord>(true);

				// Minimum y for coordinate data.
				const FWord yMin = stream.read<FWord>(true);

				// Maximum x for coordinate data.
				const FWord xMax = stream.read<FWord>(true);

				// Maximum y for coordinate data.
				const FWord yMax = stream.read<FWord>(true);

				glyph.max = {xMax, yMax};
				glyph.min = {xMin, yMin};

				// Array of last points of each contour; n is the number of contours; array entries are point indices.
				fchain<uint16> endPtsOfContours(numberOfContours);

				for (uint16& endPtOfContour : endPtsOfContours) {
					endPtOfContour = stream.read<uint16>(true);
				}

				// Total number of bytes needed for instructions.
				const uint16 instructionLength = stream.read<uint16>(true);

				stream.jump(instructionLength);

				const uint16 maxFlags = endPtsOfContours[numberOfContours - 1] + 1;

				// Array of flags.
				fchain<uint8> flags(maxFlags);

				uint16 flagIndex = 0;
				const uint16 maxIterations = maxFlags;

				for (uint16 iteration = 0; iteration < maxIterations; iteration += 1) {
					const uint8 flag = stream.read<uint8>();
					uint8 repeatCount = 0;

					if (flag & BIT(3)) {
						// If set, the next byte specifies the number of additional times this set of flags is to be
						// repeated. In this way, the number of flags listed can be smaller than the number of
						// points in a character.
						repeatCount = stream.read<uint8>();

						ENGINE_ASSERT(repeatCount > 0, "Repeat count is 0.");
					}

					/*if (flag & 0b11000000) {
						ENGINE_ASSERT(false, "[] is a reserved flag. See https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6glyf.html.", static_cast<sint32>(flag));
					}*/

					flags[flagIndex] = flag;
					flagIndex += 1;

					while (repeatCount > 0) {
						flags[flagIndex] = flag;
						flagIndex += 1;
						repeatCount -= 1;
					}
				}

				// Array of x-coordinates; the first is relative to (0,0), others are relative to previous point.
				fchain<sint16> xCoordinates(maxFlags);

				sint16 prevCoordinate = 0;

				for (sint16 xCoordinateIndex = 0; xCoordinateIndex < maxFlags; xCoordinateIndex += 1) {
					const uint8 flag = flags[xCoordinateIndex];
					sint16 xCoordinate = 0;

					if (flag & BIT(1)) {
						const sint8 sign = (flag >> 4 & BIT(0)) * 2 - 1;

						xCoordinate = static_cast<sint16>(stream.read<uint8>()) * sign;
						xCoordinate += prevCoordinate;
					}
					else {
						if (flag & BIT(4)) {
							xCoordinate = prevCoordinate;
						}
						else {
							xCoordinate = stream.read<sint16>(true);
							xCoordinate += prevCoordinate;
						}
					}

					xCoordinates[xCoordinateIndex] = xCoordinate;
					prevCoordinate = xCoordinate;
				}

				// Array of y-coordinates; the first is relative to (0,0), others are relative to previous point.
				fchain<sint16> yCoordinates(maxFlags);

				prevCoordinate = 0;

				for (sint16 yCoordinateIndex = 0; yCoordinateIndex < maxFlags; yCoordinateIndex += 1) {
					const uint8 flag = flags[yCoordinateIndex];
					sint16 yCoordinate = 0;

					if (flag & BIT(2)) {
						const sint8 sign = (flag >> 5 & BIT(0)) * 2 - 1;

						yCoordinate = static_cast<sint16>(stream.read<uint8>()) * sign;
						yCoordinate += prevCoordinate;
					}
					else {
						if (flag & BIT(5)) {
							yCoordinate = prevCoordinate;
						}
						else {
							yCoordinate = stream.read<sint16>(true);
							yCoordinate += prevCoordinate;
						}
					}

					yCoordinates[yCoordinateIndex] = yCoordinate;
					prevCoordinate = yCoordinate;
				}

				fchain<chain<EdgePoint>> contours(numberOfContours);

				for (uint16 contourIndex = 0; contourIndex < numberOfContours; contourIndex += 1) {
					const uint16 startIndex = contourIndex == 0 ? 0 : (endPtsOfContours[contourIndex - 1] + 1);
					const uint16 endIndex = endPtsOfContours[contourIndex];

					chain<EdgePoint>& contour = contours[contourIndex];

					bool previousOnCurve = flags[startIndex] & BIT(0);

					if (previousOnCurve) {
						EdgePoint point{};
						point.x = xCoordinates[startIndex];
						point.y = yCoordinates[startIndex];
						point.onCurve = true;

						contour.emplace_back(point);
					}

					for (uint16 index = startIndex + 1; index <= endIndex; index += 1) {
						EdgePoint point{};
						point.x = xCoordinates[index];
						point.y = yCoordinates[index];
						point.onCurve = flags[index] & BIT(0);

						if (!previousOnCurve && !point.onCurve) {
							// Insert an on-curve point between two off-curve points.

							EdgePoint halfwayPoint{};
							halfwayPoint.x = (xCoordinates[index - 1] + point.x) / 2;
							halfwayPoint.y = (yCoordinates[index - 1] + point.y) / 2;
							halfwayPoint.onCurve = true;

							contour.emplace_back(halfwayPoint);
						}

						contour.emplace_back(point);

						previousOnCurve = point.onCurve;
					}

					// Copy first point to close contour.
					//contour.emplace_back(contour[0]);

					contours[contourIndex] = contour;
				}

				for (const chain<EdgePoint>& contour : contours) {
					for (uint16 edgePointIndex = 1; edgePointIndex < contour.size(); edgePointIndex += 1) {
						const EdgePoint& edgePoint = contour[edgePointIndex];

						if (!edgePoint.onCurve) {
							continue;
						}

						Edge edge{};
						edge.end.x = edgePoint.x;
						edge.end.y = edgePoint.y;
						edge.isQuadraticCurve = !contour[edgePointIndex - 1].onCurve;

						if (edge.isQuadraticCurve) {
							edge.start.x = contour[edgePointIndex - 2].x;
							edge.start.y = contour[edgePointIndex - 2].y;
							edge.control.x = contour[edgePointIndex - 1].x;
							edge.control.y = contour[edgePointIndex - 1].y;
						}
						else {
							// The start point is the previous edge point.
							edge.start.x = contour[edgePointIndex - 1].x;
							edge.start.y = contour[edgePointIndex - 1].y;
						}

						glyph.edges.emplace_back(edge);
					}
				}

				BREAKPOINT();
			}
			else {
				ENGINE_WARN("Compound glyphs are not supported yet.");
			}

			glyphs[glyphIndex] = glyph;

			BREAKPOINT();
		}

		return ttfFile;
	}
}