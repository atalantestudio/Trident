// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "File/TTF/Types.hpp"

namespace Trident {
	namespace TrueTypeFont {
		// See https://learn.microsoft.com/en-us/typography/opentype/spec/gsub#gsub-header.
		struct GSUB {
			// Major version of the GSUB table.
			uint16 majorVersion;

			// Minor version of the GSUB table.
			uint16 minorVersion;

			// Offset to ScriptList table, from beginning of GSUB table.
			Offset16 scriptListOffset;

			// Offset to FeatureList table, from beginning of GSUB table.
			Offset16 featureListOffset;

			// Offset to LookupList table, from beginning of GSUB table.
			Offset16 lookupListOffset;

			// Offset to FeatureVariations table, from beginning of the GSUB table (may be NULL).
			// NOTE: Only present in the 1.1 version of this header.
			Offset32 featureVariationOffset;
		};

		struct ScriptRecord {
			// 4-byte script tag identifier.
			Tag scriptTag;

			// Offset to Script table, from beginning of ScriptList.
			Offset16 scriptOffset;
		};

		// See https://learn.microsoft.com/en-us/typography/opentype/spec/chapter2#scriptlist-table.
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

		// See https://learn.microsoft.com/en-us/typography/opentype/spec/chapter2#script-table.
		struct Script {
			// Offset to default LangSys table, from beginning of Script table — may be NULL.
			Offset16 defaultLangSysOffset;

			// Number of records in the langSysRecords array.
			uint16 langSysCount;

			// Array of LangSysRecords, sorted alphabetically by LangSys tag.
			sequence<LangSysRecord> langSysRecords;
		};

		// See https://learn.microsoft.com/en-us/typography/opentype/spec/chapter2#language-system-table.
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

		enum class GsubLookupType : uint8 {
			// Replace one glyph with one glyph.
			SINGLE = 1,

			// Replace one glyph with more than one glyph.
			MULTIPLE = 2,

			// Replace one glyph with one of many glyphs.
			_ALTERNATE = 3,

			// Replace multiple glyphs with one glyph.
			LIGATURE = 4,

			// Replace one or more glyphs in context.
			CONTEXTUAL_SUBSTITUTION = 5,

			// Replace one or more glyphs in chained context.
			CHAINED_CONTEXT_SUBSTITUTION = 6,

			// Extension mechanism for other substitution types.
			SUBSTITUTION_EXTENSION = 7,

			// Applied in reverse order, replace single glyph in chained contexts.
			REVERSE_CHAINING_CONTEXT_SINGLE = 8,
		};
	}
}