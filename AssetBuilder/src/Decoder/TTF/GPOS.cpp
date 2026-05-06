// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Decoder/TTF/GPOS.hpp"

namespace Trident {
	namespace TrueTypeFont {
		static ValueRecord decodeValueRecord(ByteStream<StreamOperation::READ>& stream, ValueFormat valueFormat) {
			ValueRecord valueRecord{};

			if (valueFormat & ValueFormat::X_PLACEMENT) {
				valueRecord.xPlacement = stream.read<sint16>();
			}

			if (valueFormat & ValueFormat::Y_PLACEMENT) {
				valueRecord.yPlacement = stream.read<sint16>();
			}

			if (valueFormat & ValueFormat::X_ADVANCE) {
				valueRecord.xAdvance = stream.read<sint16>();
			}

			if (valueFormat & ValueFormat::Y_ADVANCE) {
				valueRecord.yAdvance = stream.read<sint16>();
			}

			if (valueFormat & ValueFormat::X_PLACEMENT_DEVICE) {
				valueRecord.xPlaDeviceOffset = stream.read<Offset16>();
			}

			if (valueFormat & ValueFormat::Y_PLACEMENT_DEVICE) {
				valueRecord.yPlaDeviceOffset = stream.read<Offset16>();
			}

			if (valueFormat & ValueFormat::X_ADVANCE_DEVICE) {
				valueRecord.xAdvDeviceOffset = stream.read<Offset16>();
			}

			if (valueFormat & ValueFormat::Y_ADVANCE_DEVICE) {
				valueRecord.yAdvDeviceOffset = stream.read<Offset16>();
			}

			return valueRecord;
		}

		// TODO(filesystem/ttf/gpos/single-pos): Keep decoding SinglePos subtable?
		static void decodeSinglePosSubtable(ByteStream<StreamOperation::READ>& stream) {
			SinglePosFormat1 singlePosSubtable{};
			singlePosSubtable.format = stream.read<uint16>();

			ENGINE_ASSERT(singlePosSubtable.format == 1, "Unsupported SinglePos format.");

			singlePosSubtable.coverageOffset = stream.read<Offset16>();
			singlePosSubtable.valueFormat = static_cast<ValueFormat>(stream.read<uint16>());
			singlePosSubtable.valueRecord = decodeValueRecord(stream, singlePosSubtable.valueFormat);

			// TODO(filesystem/ttf/gpos/single-pos)
			ENGINE_ERROR_UNIMPLEMENTED();
		}

		static void insertKerningPairs(uint16 firstGlyph, view<PairSet> pairSets, File<FileFormat::TTF>& file) {
			for (const PairSet& pairSet : pairSets) {
				for (const PairValue& pairValue : pairSet.pairValueRecords) {
					const GlyphPair glyphPair = {firstGlyph, pairValue.secondGlyph};
					const KerningPair kerningPair = {pairValue.valueRecord1.xAdvance, pairValue.valueRecord2.xAdvance};

					// TODO(debug)
					DEBUG_ASSERT(file.kerningPairs.find(glyphPair) == file.kerningPairs.end());

					file.kerningPairs.insert({glyphPair, kerningPair});
				}
			}
		}

		static void decodePairPosSubtable(ByteStream<StreamOperation::READ>& stream, File<FileFormat::TTF>& file) {
			const uint64 offset = stream.getByteOffset();

			PairPosFormat1 pairPosSubtable{};
			pairPosSubtable.format = stream.read<uint16>();

			ENGINE_ASSERT(pairPosSubtable.format == 1, "Unsupported PairPos format.");

			pairPosSubtable.coverageOffset = stream.read<Offset16>();
			pairPosSubtable.valueFormat1 = static_cast<ValueFormat>(stream.read<uint16>());
			pairPosSubtable.valueFormat2 = static_cast<ValueFormat>(stream.read<uint16>());
			pairPosSubtable.pairSetCount = stream.read<uint16>();
			pairPosSubtable.pairSetOffsets = sequence<Offset16>(pairPosSubtable.pairSetCount);

			for (Offset16& pairSetOffset : pairPosSubtable.pairSetOffsets) {
				pairSetOffset = stream.read<Offset16>();
			}

			stream.seek(offset + pairPosSubtable.coverageOffset);

			Coverage coverage{};
			coverage.format = stream.read<uint16>();

			switch (coverage.format) {
				case 1: {
					coverage.coverageFormat1.glyphCount = stream.read<uint16>();
					coverage.coverageFormat1.glyphArray = sequence<uint16>(coverage.coverageFormat1.glyphCount);

					for (uint16& glyph : coverage.coverageFormat1.glyphArray) {
						glyph = stream.read<uint16>();
					}

					break;
				}
				case 2: {
					coverage.coverageFormat2.rangeCount = stream.read<uint16>();
					coverage.coverageFormat2.rangeRecords = sequence<RangeRecord>(coverage.coverageFormat2.rangeCount);

					for (RangeRecord& rangeRecord : coverage.coverageFormat2.rangeRecords) {
						rangeRecord.startGlyphID = stream.read<uint16>();
						rangeRecord.endGlyphID = stream.read<uint16>();
						rangeRecord.startCoverageIndex = stream.read<uint16>();
					}

					break;
				}
				default: {
					ENGINE_ASSERT(false, "Invalid coverage table format.");

					break;
				}
			}

			sequence<PairSet> pairSets(pairPosSubtable.pairSetCount);
			uint16 pairSetIndex = 0;

			while (pairSetIndex < pairPosSubtable.pairSetCount) {
				const Offset16 pairSetOffset = pairPosSubtable.pairSetOffsets[pairSetIndex];

				stream.seek(offset + pairSetOffset);

				PairSet& pairSet = pairSets[pairSetIndex];
				pairSet.pairValueCount = stream.read<uint16>();
				pairSet.pairValueRecords = sequence<PairValue>(pairSet.pairValueCount);

				for (PairValue& pairValue : pairSet.pairValueRecords) {
					pairValue.secondGlyph = stream.read<uint16>();
					pairValue.valueRecord1 = decodeValueRecord(stream, pairPosSubtable.valueFormat1);
					pairValue.valueRecord2 = decodeValueRecord(stream, pairPosSubtable.valueFormat2);
				}

				pairSetIndex += 1;
			}

			if (pairPosSubtable.valueFormat1 & ValueFormat::X_ADVANCE) {
				switch (coverage.format) {
					case 1: {
						for (const uint16 firstGlyph : coverage.coverageFormat1.glyphArray) {
							insertKerningPairs(firstGlyph, pairSets, file);
						}

						break;
					}
					case 2: {
						for (const RangeRecord& rangeRecord : coverage.coverageFormat2.rangeRecords) {
							uint16 firstGlyph = rangeRecord.startGlyphID;

							while (firstGlyph <= rangeRecord.endGlyphID) {
								insertKerningPairs(firstGlyph, pairSets, file);

								firstGlyph += 1;
							}
						}

						break;
					}
					default: {
						DEBUG_ASSERT(false);

						break;
					}
				}
			}
		}

		static void decodePosExtensionSubtable(ByteStream<StreamOperation::READ>& stream, File<FileFormat::TTF>& file) {
			const uint64 offset = stream.getByteOffset();

			PosExtensionFormat1 posExtensionSubtable{};
			posExtensionSubtable.format = stream.read<uint16>();

			ENGINE_ASSERT(posExtensionSubtable.format == 1, "Unsupported PosExtension format.");

			posExtensionSubtable.extensionLookupType = static_cast<LookupType>(stream.read<uint16>());
			posExtensionSubtable.extensionOffset = stream.read<Offset32>();

			stream.seek(offset + posExtensionSubtable.extensionOffset);

			switch (posExtensionSubtable.extensionLookupType) {
				case LookupType::SINGLE_ADJUSTMENT: {
					decodeSinglePosSubtable(stream);

					break;
				}
				case LookupType::PAIR_ADJUSTMENT: {
					decodePairPosSubtable(stream, file);

					break;
				}
				case LookupType::POSITIONING_EXTENSION: {
					ENGINE_ASSERT(false, "Invalid lookup type.");

					break;
				}

				// TODO(filesystem/ttf/gpos/lookup-type): Add support for other lookup types.
				default: {
					break;
				}
			}
		}

		void decodeGPOSTable(ByteStream<StreamOperation::READ>& stream, const std::unordered_map<uint32, uint32>& tableOffsetMap, File<FileFormat::TTF>& file) {
			const std::unordered_map<uint32, uint32>::const_iterator tableOffset = tableOffsetMap.find(STRING_TO_LITTLE_ENDIAN_UINT32("GPOS"));

			ENGINE_ASSERT(tableOffset != tableOffsetMap.end(), "Could not find 'GPOS' table.");

			stream.seek(tableOffset->second);

			GPOS gpos{};
			gpos.majorVersion = stream.read<uint16>();
			gpos.minorVersion = stream.read<uint16>();
			gpos.scriptListOffset = stream.read<Offset16>();
			gpos.featureListOffset = stream.read<Offset16>();
			gpos.lookupListOffset = stream.read<Offset16>();

			if (gpos.majorVersion == 1 && gpos.minorVersion == 1) {
				gpos.featureVariationOffset = stream.read<Offset32>();
			}

			// TODO(filesystem/ttf/gpos/script): Should scripts be decoded along with features and lookups?
			stream.seek(tableOffset->second + gpos.scriptListOffset);

			ScriptList scriptList{};
			scriptList.scriptCount = stream.read<uint16>();
			scriptList.scriptRecords = sequence<ScriptRecord>(scriptList.scriptCount);

			for (ScriptRecord& scriptRecord : scriptList.scriptRecords) {
				scriptRecord.scriptTag = stream.read<Tag>(true);
				scriptRecord.scriptOffset = stream.read<Offset16>();
			}

			sequence<Script> scripts(scriptList.scriptCount);

			// Parse scripts.
			{
				uint16 scriptIndex = 0;

				while (scriptIndex < scriptList.scriptCount) {
					const ScriptRecord& scriptRecord = scriptList.scriptRecords[scriptIndex];

					stream.seek(tableOffset->second + gpos.scriptListOffset + scriptRecord.scriptOffset);

					Script& script = scripts[scriptIndex];
					script.defaultLangSysOffset = stream.read<Offset16>();
					script.langSysCount = stream.read<uint16>();
					script.langSysRecords = sequence<LangSysRecord>(script.langSysCount);

					for (LangSysRecord& langSysRecord : script.langSysRecords) {
						langSysRecord.langSysTag = stream.read<Tag>(true);
						langSysRecord.langSysOffset = stream.read<Offset16>();
					}

					for (const LangSysRecord& langSysRecord : script.langSysRecords) {
						stream.seek(tableOffset->second + gpos.scriptListOffset + scriptRecord.scriptOffset + langSysRecord.langSysOffset);
						stream.jump(sizeof(LangSys::lookupOrderOffset));

						LangSys langSys{};
						langSys.requiredFeatureIndex = stream.read<uint16>();
						langSys.featureIndexCount = stream.read<uint16>();
						langSys.featureIndices = sequence<uint16>(langSys.featureIndexCount);

						for (uint16& featureIndex : langSys.featureIndices) {
							featureIndex = stream.read<uint16>();
						}
					}

					scriptIndex += 1;
				}
			}

			stream.seek(tableOffset->second + gpos.lookupListOffset);

			LookupList lookupList{};
			lookupList.lookupCount = stream.read<uint16>();
			lookupList.lookupOffsets = sequence<Offset16>(lookupList.lookupCount);

			for (Offset16& lookupOffset : lookupList.lookupOffsets) {
				lookupOffset = stream.read<Offset16>();
			}

			sequence<Lookup> lookups(lookupList.lookupCount);

			// Parse lookups.
			{
				uint16 lookupIndex = 0;

				while (lookupIndex < lookupList.lookupCount) {
					const Offset16 lookupOffset = lookupList.lookupOffsets[lookupIndex];

					stream.seek(tableOffset->second + gpos.lookupListOffset + lookupOffset);

					Lookup& lookup = lookups[lookupIndex];
					lookup.lookupType = static_cast<LookupType>(stream.read<uint16>());
					lookup.lookupFlag = static_cast<LookupFlag>(stream.read<uint16>());
					lookup.subtableCount = stream.read<uint16>();
					lookup.subtableOffsets = sequence<Offset16>(lookup.subtableCount);

					for (Offset16& subtableOffset : lookup.subtableOffsets) {
						subtableOffset = stream.read<Offset16>();
					}

					if (lookup.lookupFlag & LookupFlag::USE_MARK_FILTERING_SET) {
						lookup.markFilteringSet = stream.read<uint16>();
					}

					lookupIndex += 1;
				}
			}

			stream.seek(tableOffset->second + gpos.featureListOffset);

			FeatureList featureList{};
			featureList.featureCount = stream.read<uint16>();
			featureList.featureRecords = sequence<FeatureRecord>(featureList.featureCount);

			for (FeatureRecord& featureRecord : featureList.featureRecords) {
				featureRecord.featureTag = stream.read<Tag>(true);
				featureRecord.featureOffset = stream.read<Offset16>();
			}

			sequence<Feature> features(featureList.featureCount);

			// Parse features.
			{
				uint16 featureIndex = 0;

				while (featureIndex < featureList.featureCount) {
					const FeatureRecord& featureRecord = featureList.featureRecords[featureIndex];

					stream.seek(tableOffset->second + gpos.featureListOffset + featureRecord.featureOffset);

					Feature& feature = features[featureIndex];
					feature.featureParamsOffset = stream.read<Offset16>();
					feature.lookupIndexCount = stream.read<uint16>();
					feature.lookupListIndices = sequence<uint16>(feature.lookupIndexCount);

					for (uint16& lookupListIndex : feature.lookupListIndices) {
						lookupListIndex = stream.read<uint16>();
					}

					featureIndex += 1;
				}
			}

			uint16 featureIndex = 0;

			while (featureIndex < featureList.featureCount) {
				const FeatureRecord& featureRecord = featureList.featureRecords[featureIndex];

				if (featureRecord.featureTag != STRING_TO_LITTLE_ENDIAN_UINT32("kern")) {
					featureIndex += 1;

					continue;
				}

				const Feature& feature = features[featureIndex];

				for (const uint16 lookupIndex : feature.lookupListIndices) {
					const Offset16 lookupOffset = lookupList.lookupOffsets[lookupIndex];
					const Lookup& lookup = lookups[lookupIndex];
					const uint32 offset = tableOffset->second + gpos.lookupListOffset + lookupOffset;

					switch (lookup.lookupType) {
						case LookupType::SINGLE_ADJUSTMENT: {
							for (const Offset16 subtableOffset : lookup.subtableOffsets) {
								stream.seek(offset + subtableOffset);

								decodeSinglePosSubtable(stream);
							}

							break;
						}
						case LookupType::PAIR_ADJUSTMENT: {
							for (const Offset16 subtableOffset : lookup.subtableOffsets) {
								stream.seek(offset + subtableOffset);

								decodePairPosSubtable(stream, file);
							}

							break;
						}
						case LookupType::POSITIONING_EXTENSION: {
							for (const Offset16 subtableOffset : lookup.subtableOffsets) {
								stream.seek(offset + subtableOffset);

								decodePosExtensionSubtable(stream, file);
							}

							break;
						}

						// TODO(filesystem/ttf/gpos/lookup-type): Add support for other lookup types.
						default: {
							break;
						}
					}
				}

				featureIndex += 1;
			}

			BREAKPOINT();
		}
	}
}