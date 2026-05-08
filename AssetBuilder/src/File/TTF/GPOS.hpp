// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "File/TTF/Types.hpp"

namespace Trident {
	namespace TrueTypeFont {
		/// See https://learn.microsoft.com/en-us/typography/opentype/spec/gpos#gpos-header.
		struct GPOS {
			// Major version of the GPOS table.
			uint16 majorVersion;

			// Minor version of the GPOS table.
			uint16 minorVersion;

			// Offset to ScriptList table, from beginning of GPOS table.
			Offset16 scriptListOffset;

			// Offset to FeatureList table, from beginning of GPOS table.
			Offset16 featureListOffset;

			// Offset to LookupList table, from beginning of GPOS table.
			Offset16 lookupListOffset;

			// Offset to FeatureVariations table, from beginning of GPOS table (may be NULL).
			Offset32 featureVariationOffset;
		};

		struct ScriptRecord {
			// 4-byte script tag identifier.
			Tag scriptTag;

			// Offset to Script table, from beginning of ScriptList.
			Offset16 scriptOffset;
		};

		/// See https://learn.microsoft.com/en-us/typography/opentype/spec/chapter2#scriptlist-table.
		struct ScriptList {
			// Number of ScriptRecords.
			uint16 scriptCount;

			// Array of ScriptRecords, listed alphabetically by script tag.
			sequence<ScriptRecord> scriptRecords;
		};

		struct LangSysRecord {
			// 4-byte LangSysTag identifier.
			Tag langSysTag;

			// Offset to LangSys table, from beginning of Script table.
			Offset16 langSysOffset;
		};

		/// See https://learn.microsoft.com/en-us/typography/opentype/spec/chapter2#script-table.
		struct Script {
			// Offset to default LangSys table, from beginning of Script table — may be NULL.
			Offset16 defaultLangSysOffset;

			// Number of records in the langSysRecords array.
			uint16 langSysCount;

			// Array of LangSysRecords, sorted alphabetically by LangSys tag.
			sequence<LangSysRecord> langSysRecords;
		};

		/// See https://learn.microsoft.com/en-us/typography/opentype/spec/chapter2#language-system-table.
		struct LangSys {
			// Reserved—set to NULL.
			Offset16 lookupOrderOffset;

			// Index of a feature required for this language system; if no required features, set to 0xFFFF.
			uint16 requiredFeatureIndex;

			// Number of elements in the featureIndices array.
			uint16 featureIndexCount;

			// Array of indices into the FeatureList, in arbitrary order.
			sequence<uint16> featureIndices;
		};

		struct FeatureRecord {
			// 4-byte feature identification tag.
			Tag featureTag;

			// Offset to Feature table, from beginning of FeatureList.
			Offset16 featureOffset;
		};

		/// See https://learn.microsoft.com/en-us/typography/opentype/spec/chapter2#featurelist-table.
		struct FeatureList {
			// Number of records in the featureRecords array.
			uint16 featureCount;

			// Array of FeatureRecords.
			sequence<FeatureRecord> featureRecords;
		};

		/// See https://learn.microsoft.com/en-us/typography/opentype/spec/chapter2#feature-table.
		struct Feature {
			// Offset from start of Feature table to feature parameters table, if defined for the feature and present, else NULL.
			Offset16 featureParamsOffset;

			// Number of elements in the lookupListIndices array.
			uint16 lookupIndexCount;

			// Array of indices into the LookupList — zero-based.
			sequence<uint16> lookupListIndices;
		};

		/// See https://learn.microsoft.com/en-us/typography/opentype/spec/chapter2#lookuplist-table.
		struct LookupList {
			// Number of elements in the lookupOffsets array.
			uint16 lookupCount;

			// Array of offsets to Lookup tables, from beginning of LookupList.
			sequence<Offset16> lookupOffsets;
		};

		enum class LookupType : uint16 {
			// Adjust position of a single glyph.
			SINGLE_ADJUSTMENT = 1,

			// Adjust position of a pair of glyphs.
			PAIR_ADJUSTMENT = 2,

			// Extension mechanism for other positionings.
			POSITIONING_EXTENSION = 9,
		};

		enum class LookupFlag : uint16 {
			// This bit is used only in relation to cursive attachment positioning (GPOS lookup type 3).
			// When this bit is set, the last glyph in a matched input sequence will be positioned on the baseline.
			RIGHT_TO_LEFT = 0x0001,

			// If set, skips over base glyphs.
			IGNORE_BASE_GLYPHS = 0x0002,

			// If set, skips over ligatures.
			IGNORE_LIGATURES = 0x0004,

			// If set, skips over all combining marks.
			IGNORE_MARKS = 0x0008,

			// If set, indicates that the Lookup table structure includes the markFilteringSet field.
			// The layout engine skips over all mark glyphs not in the mark filtering set indicated.
			USE_MARK_FILTERING_SET = 0x0010,

			// If not zero, skips over all marks not in the specified mark attachment class.
			MARK_ATTACHMENT_CLASS_FILTER = 0xFF00,
		};

		AS_FLAG(LookupFlag);

		/// See https://learn.microsoft.com/en-us/typography/opentype/spec/chapter2#lookup-table.
		struct Lookup {
			// Different enumerations for GSUB and GPOS.
			LookupType lookupType;

			// Lookup qualifiers.
			LookupFlag lookupFlag;

			// Number of elements in the subtableOffsets array.
			uint16 subtableCount;

			// Array of offsets to lookup subtables, from beginning of Lookup table.
			sequence<Offset16> subtableOffsets;

			// Index (base 0) into GDEF mark glyph sets structure.
			// This field is only present if the USE_MARK_FILTERING_SET lookup flag is set.
			uint16 markFilteringSet;
		};

		struct RangeRecord {
			// First glyph ID in the range.
			uint16 startGlyphID;

			// Last glyph ID in the range.
			uint16 endGlyphID;

			// Coverage Index of first glyph ID in range.
			uint16 startCoverageIndex;
		};

		/// See https://learn.microsoft.com/en-us/typography/opentype/spec/chapter2#coverage-format-1.
		struct CoverageFormat1 {
			// Number of glyphs in the glyph array.
			uint16 glyphCount;

			// Array of glyph IDs — in numerical order.
			sequence<uint16> glyphArray;
		};

		/// See https://learn.microsoft.com/en-us/typography/opentype/spec/chapter2#coverage-format-2.
		struct CoverageFormat2 {
			// Number of RangeRecords.
			uint16 rangeCount;

			// Array of glyph ranges — ordered by startGlyphID.
			sequence<RangeRecord> rangeRecords;
		};

		/// See https://learn.microsoft.com/en-us/typography/opentype/spec/chapter2#coverage-table.
		struct Coverage {
			// Format identifier — format = 1 or 2.
			uint16 format;

			CoverageFormat1 coverageFormat1;
			CoverageFormat2 coverageFormat2;
		};

		/// See https://learn.microsoft.com/en-us/typography/opentype/spec/gpos#value-record.
		struct ValueRecord {
			// Horizontal adjustment for placement, in design units.
			sint16 xPlacement;

			// Vertical adjustment for placement, in design units.
			sint16 yPlacement;

			// Horizontal adjustment for advance, in design units — only used for horizontal layout.
			sint16 xAdvance;

			// Vertical adjustment for advance, in design units — only used for vertical layout.
			sint16 yAdvance;

			// Offset to Device table (non-variable font) / VariationIndex table (variable font) for horizontal placement,
			// from beginning of the immediate parent table (SinglePos or PairPosFormat2 lookup subtable,
			// PairSet table within a PairPosFormat1 lookup subtable) — may be NULL.
			Offset16 xPlaDeviceOffset;

			// Offset to Device table (non-variable font) / VariationIndex table (variable font) for vertical placement,
			// from beginning of the immediate parent table (SinglePos or PairPosFormat2 lookup subtable,
			// PairSet table within a PairPosFormat1 lookup subtable) — may be NULL.
			Offset16 yPlaDeviceOffset;

			// Offset to Device table (non-variable font) / VariationIndex table (variable font) for horizontal advance,
			// from beginning of the immediate parent table (SinglePos or PairPosFormat2 lookup subtable,
			// PairSet table within a PairPosFormat1 lookup subtable) — may be NULL.
			Offset16 xAdvDeviceOffset;

			// Offset to Device table (non-variable font) / VariationIndex table (variable font) for vertical advance,
			// from beginning of the immediate parent table (SinglePos or PairPosFormat2 lookup subtable,
			// PairSet table within a PairPosFormat1 lookup subtable) — may be NULL.
			Offset16 yAdvDeviceOffset;
		};

		enum class ValueFormat : uint16 {
			// Includes horizontal adjustment for placement.
			X_PLACEMENT = 0x0001,

			// Includes vertical adjustment for placement.
			Y_PLACEMENT = 0x0002,

			// Includes horizontal adjustment for advance.
			X_ADVANCE = 0x0004,

			// Includes vertical adjustment for advance.
			Y_ADVANCE = 0x0008,

			// Includes Device table (non-variable font) / VariationIndex table (variable font) for horizontal placement.
			X_PLACEMENT_DEVICE = 0x0010,

			// Includes Device table (non-variable font) / VariationIndex table (variable font) for vertical placement.
			Y_PLACEMENT_DEVICE = 0x0020,

			// Includes Device table (non-variable font) / VariationIndex table (variable font) for horizontal advance.
			X_ADVANCE_DEVICE = 0x0040,

			// Includes Device table (non-variable font) / VariationIndex table (variable font) for vertical advance.
			Y_ADVANCE_DEVICE = 0x0080,
		};

		AS_FLAG(ValueFormat);

		struct PairValue {
			// Glyph ID of second glyph in the pair (first glyph is listed in the Coverage table).
			uint16 secondGlyph;

			// Positioning data for the first glyph in the pair.
			ValueRecord valueRecord1;

			// Positioning data for the second glyph in the pair.
			ValueRecord valueRecord2;
		};

		struct PairSet {
			// Number of PairValue records.
			uint16 pairValueCount;

			// Array of PairValue records, ordered by glyph ID of the second glyph.
			sequence<PairValue> pairValueRecords;
		};

		struct ClassRange {
			// First glyph ID in the range.
			uint16 startGlyphID;

			// Last glyph ID in the range.
			uint16 endGlyphID;

			// Applied to all glyphs in the range.
			uint16 _class;
		};

		struct Class2 {
			// Positioning for first glyph — empty if valueFormat1 = 0.
			ValueRecord valueRecord1;

			// Positioning for second glyph — empty if valueFormat2 = 0.
			ValueRecord valueRecord2;
		};

		struct Class1 {
			// Array of Class2 records, ordered by classes in classDef2.
			sequence<Class2> class2Records;
		};

		/// See https://learn.microsoft.com/en-us/typography/opentype/spec/chapter2#class-definition-format-1.
		struct ClassDefFormat1 {
			// First glyph ID assigned to a class.
			uint16 startGlyphID;

			// Number of elements in the classValues array.
			uint16 glyphCount;

			// Array of class values — one per glyph ID.
			sequence<uint16> classValues;
		};

		/// See https://learn.microsoft.com/en-us/typography/opentype/spec/chapter2#class-definition-format-2.
		struct ClassDefFormat2 {
			// Number of ClassRange records.
			uint16 classRangeCount;

			// Array of ClassRangeRecords — ordered by startGlyphID.
			sequence<ClassRange> classRangeRecords;
		};

		struct ClassDef {
			// Format identifier.
			uint16 format;

			ClassDefFormat1 classDefFormat1;
			ClassDefFormat2 classDefFormat2;
		};

		/// See https://learn.microsoft.com/en-us/typography/opentype/spec/gpos#lookup-type-1-subtable-single-adjustment-positioning.
		struct SinglePosFormat1 {
			// Format identifier — format = 1.
			uint16 format;

			// Offset to Coverage table, from beginning of SinglePos subtable.
			Offset16 coverageOffset;

			// Defines the types of data in the ValueRecord.
			ValueFormat valueFormat;

			// Defines positioning value(s) — applied to all glyphs in the Coverage table.
			ValueRecord valueRecord;
		};

		struct PairPosFormat1 {
			// Number of PairSet tables.
			uint16 pairSetCount;

			// Array of offsets to PairSet tables. Offsets are from beginning of PairPos subtable, ordered by Coverage Index.
			sequence<Offset16> pairSetOffsets;
		};

		struct PairPosFormat2 {
			// Offset to ClassDef table, from beginning of PairPos subtable — for the first glyph of the pair.
			Offset16 classDef1Offset;

			// Offset to ClassDef table, from beginning of PairPos subtable — for the second glyph of the pair.
			Offset16 classDef2Offset;

			// Number of classes in classDef1 table — includes class 0.
			uint16 class1Count;

			// Number of classes in classDef2 table — includes class 0.
			uint16 class2Count;

			// Array of Class1 records, ordered by classes in classDef1.
			sequence<Class1> class1Records;
		};

		/// See https://learn.microsoft.com/en-us/typography/opentype/spec/gpos#lookup-type-2-subtable-pair-adjustment-positioning.
		struct PairPos {
			// Format identifier.
			uint16 format;

			// Offset to Coverage table, from beginning of PairPos subtable.
			Offset16 coverageOffset;

			// ValueRecord definition — for the first glyph of the pair (may be zero).
			ValueFormat valueFormat1;

			// ValueRecord definition — for the second glyph of the pair (may be zero).
			ValueFormat valueFormat2;

			PairPosFormat1 pairPosFormat1;
			PairPosFormat2 pairPosFormat2;
		};

		struct PosExtensionFormat1 {
			// Lookup type of subtable referenced by extensionOffset (i.e. the extension subtable).
			LookupType extensionLookupType;

			// Offset to the extension subtable, of lookup type extensionLookupType, relative to the start of the PosExtensionFormat1 subtable.
			Offset32 extensionOffset;
		};

		/// See https://learn.microsoft.com/en-us/typography/opentype/spec/gpos#lookup-type-9-subtable-positioning-subtable-extension.
		struct PosExtension {
			// Format identifier.
			uint16 format;

			PosExtensionFormat1 posExtensionFormat1;
		};
	}
}