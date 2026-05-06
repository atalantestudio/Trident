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
			// NOTE: Only present in the 1.1 version of this header.
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
			uint16 lookupType;

			// Lookup qualifiers.
			uint16 lookupFlag;

			// Number of elements in the subtableOffsets array.
			uint16 subtableCount;

			// Array of offsets to lookup subtables, from beginning of Lookup table.
			sequence<Offset16> subtableOffsets;

			// Index (base 0) into GDEF mark glyph sets structure.
			// This field is only present if the USE_MARK_FILTERING_SET lookup flag is set.
			uint16 markFilteringSet;
		};
	}
}