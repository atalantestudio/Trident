// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Decoder/TTF/GPOS.hpp"

namespace Trident {
	namespace TrueTypeFont {
		static Script decodeScript(ByteStream<StreamOperation::READ>& stream) {
			Script script{};
			script.defaultLangSysOffset = stream.read<Offset16>();
			script.langSysCount = stream.read<uint16>();
			script.langSysRecords = sequence<LangSysRecord>(script.langSysCount);

			for (LangSysRecord& langSysRecord : script.langSysRecords) {
				langSysRecord.langSysTag = stream.read<Tag>(true);
				langSysRecord.langSysOffset = stream.read<Offset16>();
			}

			return script;
		}

		static LangSys decodeLangSys(ByteStream<StreamOperation::READ>& stream) {
			stream.jump(sizeof(LangSys::lookupOrderOffset));

			LangSys langSys{};
			langSys.requiredFeatureIndex = stream.read<uint16>();
			langSys.featureIndexCount = stream.read<uint16>();
			langSys.featureIndices = sequence<uint16>(langSys.featureIndexCount);

			for (uint16& featureIndex : langSys.featureIndices) {
				featureIndex = stream.read<uint16>();
			}

			return langSys;
		}

		static Lookup decodeLookup(ByteStream<StreamOperation::READ>& stream) {
			Lookup lookup{};
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

			return lookup;
		}

		static Feature decodeFeature(ByteStream<StreamOperation::READ>& stream) {
			Feature feature{};
			feature.featureParamsOffset = stream.read<Offset16>();
			feature.lookupIndexCount = stream.read<uint16>();
			feature.lookupListIndices = sequence<uint16>(feature.lookupIndexCount);

			for (uint16& lookupListIndex : feature.lookupListIndices) {
				lookupListIndex = stream.read<uint16>();
			}

			return feature;
		}

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

		static ClassDef decodeClassDef(ByteStream<StreamOperation::READ>& stream) {
			ClassDef classDef;
			classDef.format = stream.read<uint16>();

			switch (classDef.format) {
				case 1: {
					classDef.classDefFormat1.startGlyphID = stream.read<uint16>();
					classDef.classDefFormat1.glyphCount = stream.read<uint16>();
					classDef.classDefFormat1.classValues = sequence<uint16>(classDef.classDefFormat1.glyphCount);

					for (uint16& classValue : classDef.classDefFormat1.classValues) {
						classValue = stream.read<uint16>();
					}

					break;
				}
				case 2: {
					classDef.classDefFormat2.classRangeCount = stream.read<uint16>();
					classDef.classDefFormat2.classRangeRecords = sequence<ClassRange>(classDef.classDefFormat2.classRangeCount);

					for (ClassRange& classRange : classDef.classDefFormat2.classRangeRecords) {
						classRange.startGlyphID = stream.read<uint16>();
						classRange.endGlyphID = stream.read<uint16>();
						classRange.class_ = stream.read<uint16>();
					}

					break;
				}
				default: {
					ENGINE_ASSERT(false, "Unsupported ClassDef format.");

					break;
				}
			}

			return classDef;
		}

		static void insertKerningPair(uint16 firstGlyph, const PairValue& pairValue, File<FileFormat::TTF>& file) {
			const GlyphPair glyphPair = {firstGlyph, pairValue.secondGlyph};

			DEBUG_ASSERT(file.kerningPairs.find(glyphPair) == file.kerningPairs.end());

			file.kerningPairs.insert({glyphPair, pairValue.valueRecord1.xAdvance});
		}

		static void insertKerningPair(uint16 firstGlyph, uint16 secondGlyph, const Class2& class2, File<FileFormat::TTF>& file) {
			const GlyphPair glyphPair = {firstGlyph, secondGlyph};

			DEBUG_ASSERT(file.kerningPairs.find(glyphPair) == file.kerningPairs.end());

			file.kerningPairs.insert({glyphPair, class2.valueRecord1.xAdvance});
		}

		static void decodePairPosSubtable(ByteStream<StreamOperation::READ>& stream, File<FileFormat::TTF>& file) {
			const uint64 offset = stream.getByteOffset();

			PairPos pairPosSubtable{};
			pairPosSubtable.format = stream.read<uint16>();
			pairPosSubtable.coverageOffset = stream.read<Offset16>();
			pairPosSubtable.valueFormat1 = static_cast<ValueFormat>(stream.read<uint16>());
			pairPosSubtable.valueFormat2 = static_cast<ValueFormat>(stream.read<uint16>());

			sequence<PairSet> pairSets;
			ClassDef classDef1;
			ClassDef classDef2;

			switch (pairPosSubtable.format) {
				case 1: {
					pairPosSubtable.pairPosFormat1.pairSetCount = stream.read<uint16>();
					pairPosSubtable.pairPosFormat1.pairSetOffsets = sequence<Offset16>(pairPosSubtable.pairPosFormat1.pairSetCount);

					for (Offset16& pairSetOffset : pairPosSubtable.pairPosFormat1.pairSetOffsets) {
						pairSetOffset = stream.read<Offset16>();
					}

					pairSets = sequence<PairSet>(pairPosSubtable.pairPosFormat1.pairSetCount);

					uint16 pairSetIndex = 0;

					while (pairSetIndex < pairPosSubtable.pairPosFormat1.pairSetCount) {
						const Offset16 pairSetOffset = pairPosSubtable.pairPosFormat1.pairSetOffsets[pairSetIndex];

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

					break;
				}
				/* case 2: {
					pairPosSubtable.pairPosFormat2.classDef1Offset = stream.read<Offset16>();
					pairPosSubtable.pairPosFormat2.classDef2Offset = stream.read<Offset16>();
					pairPosSubtable.pairPosFormat2.class1Count = stream.read<uint16>();
					pairPosSubtable.pairPosFormat2.class2Count = stream.read<uint16>();
					pairPosSubtable.pairPosFormat2.class1Records = sequence<Class1>(pairPosSubtable.pairPosFormat2.class1Count);

					for (Class1& class1Record : pairPosSubtable.pairPosFormat2.class1Records) {
						class1Record.class2Records = sequence<Class2>(pairPosSubtable.pairPosFormat2.class2Count);

						for (Class2& class2Record : class1Record.class2Records) {
							class2Record.valueRecord1 = decodeValueRecord(stream, pairPosSubtable.valueFormat1);
							class2Record.valueRecord2 = decodeValueRecord(stream, pairPosSubtable.valueFormat2);
						}
					}

					stream.seek(offset + pairPosSubtable.pairPosFormat2.classDef1Offset);

					classDef1 = decodeClassDef(stream);

					stream.seek(offset + pairPosSubtable.pairPosFormat2.classDef2Offset);

					classDef2 = decodeClassDef(stream);

					break;
				} */
				default: {
					ENGINE_WARNING("Encountered unsupported TTF PairPos format ([]).", pairPosSubtable.format);

					break;
				}
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

			if (!(pairPosSubtable.valueFormat1 & ValueFormat::X_ADVANCE && pairPosSubtable.valueFormat2 == 0)) {
				return;
			}

			switch (pairPosSubtable.format) {
				case 1: {
					switch (coverage.format) {
						case 1: {
							uint16 coverageIndex = 0;

							while (coverageIndex < coverage.coverageFormat1.glyphCount) {
								const uint16 firstGlyph = coverage.coverageFormat1.glyphArray[coverageIndex];
								const PairSet& pairSet = pairSets[coverageIndex];

								for (const PairValue& pairValue : pairSet.pairValueRecords) {
									insertKerningPair(firstGlyph, pairValue, file);
								}

								coverageIndex += 1;
							}

							break;
						}
						case 2: {
							for (const RangeRecord& rangeRecord : coverage.coverageFormat2.rangeRecords) {
								uint16 firstGlyph = rangeRecord.startGlyphID;

								while (firstGlyph <= rangeRecord.endGlyphID) {
									const uint16 coverageIndex = rangeRecord.startCoverageIndex + firstGlyph - rangeRecord.startGlyphID;
									const PairSet& pairSet = pairSets[coverageIndex];

									for (const PairValue& pairValue : pairSet.pairValueRecords) {
										insertKerningPair(firstGlyph, pairValue, file);
									}

									firstGlyph += 1;
								}
							}

							break;
						}
						default: {
							break;
						}
					}

					break;
				}
				/* case 2: {
					switch (classDef1.format) {
						case 1: {
							uint16 firstGlyphOffset = 0;

							while (firstGlyphOffset < classDef1.classDefFormat1.glyphCount) {
								const uint16 classValue = classDef1.classDefFormat1.classValues[firstGlyphOffset];

								if (classValue == 0) {
									firstGlyphOffset += 1;

									continue;
								}

								const uint16 firstGlyph = firstGlyphOffset + classDef1.classDefFormat1.startGlyphID;
								const Class1& class1 = pairPosSubtable.pairPosFormat2.class1Records[classValue - 1];

								switch (classDef2.format) {
									case 1: {
										uint16 secondGlyphOffset = 0;

										while (secondGlyphOffset < classDef2.classDefFormat1.glyphCount) {
											const uint16 classValue = classDef2.classDefFormat1.classValues[secondGlyphOffset];

											if (classValue == 0) {
												secondGlyphOffset += 1;

												continue;
											}

											const uint16 secondGlyph = secondGlyphOffset + classDef2.classDefFormat1.startGlyphID;
											const Class2& class2 = class1.class2Records[classValue - 1];

											insertKerningPair(firstGlyph, secondGlyph, class2, file);

											secondGlyphOffset += 1;
										}

										break;
									}
									case 2: {
										for (const ClassRange& classRange2 : classDef2.classDefFormat2.classRangeRecords) {
											DEBUG_ASSERT(classRange2.class_ > 0);

											const Class2& class2 = class1.class2Records[classRange2.class_];

											uint16 secondGlyph = classRange2.startGlyphID;

											while (secondGlyph <= classRange2.endGlyphID) {
												insertKerningPair(firstGlyph, secondGlyph, class2, file);

												secondGlyph += 1;
											}
										}

										break;
									}
									default: {
										break;
									}
								}

								firstGlyphOffset += 1;
							}

							break;
						}
						case 2: {
							for (const ClassRange& classRange1 : classDef1.classDefFormat2.classRangeRecords) {
								DEBUG_ASSERT(classRange1.class_ > 0);

								const Class1& class1 = pairPosSubtable.pairPosFormat2.class1Records[classRange1.class_];

								uint16 firstGlyph = classRange1.startGlyphID;

								switch (classDef2.format) {
									case 1: {
										while (firstGlyph <= classRange1.endGlyphID) {
											uint16 secondGlyphOffset = 0;

											while (secondGlyphOffset < classDef2.classDefFormat1.glyphCount) {
												const uint16 classValue = classDef2.classDefFormat1.classValues[secondGlyphOffset];

												if (classValue == 0) {
													secondGlyphOffset += 1;

													continue;
												}

												const uint16 secondGlyph = secondGlyphOffset + classDef2.classDefFormat1.startGlyphID;
												const Class2& class2 = class1.class2Records[classValue - 1];

												insertKerningPair(firstGlyph, secondGlyph, class2, file);

												secondGlyphOffset += 1;
											}

											firstGlyph += 1;
										}

										break;
									}
									case 2: {
										while (firstGlyph <= classRange1.endGlyphID) {
											for (const ClassRange& classRange2 : classDef2.classDefFormat2.classRangeRecords) {
												DEBUG_ASSERT(classRange2.class_ > 0);

												const Class2& class2 = class1.class2Records[classRange2.class_];

												uint16 secondGlyph = classRange2.startGlyphID;

												while (secondGlyph <= classRange2.endGlyphID) {
													insertKerningPair(firstGlyph, secondGlyph, class2, file);

													secondGlyph += 1;
												}
											}

											firstGlyph += 1;
										}

										break;
									}
									default: {
										break;
									}
								}
							}

							break;
						}
						default: {
							break;
						}
					}

					break;
				} */
				default: {
					break;
				}
			}
		}

		static void decodePosExtensionSubtable(ByteStream<StreamOperation::READ>& stream, File<FileFormat::TTF>& file) {
			const uint64 offset = stream.getByteOffset();

			PosExtension posExtensionSubtable{};
			posExtensionSubtable.format = stream.read<uint16>();

			ENGINE_ASSERT(posExtensionSubtable.format == 1, "Unsupported PosExtension format.");

			posExtensionSubtable.posExtensionFormat1.extensionLookupType = static_cast<LookupType>(stream.read<uint16>());
			posExtensionSubtable.posExtensionFormat1.extensionOffset = stream.read<Offset32>();

			stream.seek(offset + posExtensionSubtable.posExtensionFormat1.extensionOffset);

			switch (posExtensionSubtable.posExtensionFormat1.extensionLookupType) {
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

		static void applyDefaultLangSys(
			ByteStream<StreamOperation::READ>& stream,
			uint32 tableOffset,
			const GPOS& gpos,
			const ScriptList& scriptList,
			const LookupList& lookupList,
			const FeatureList& featureList,
			File<FileFormat::TTF>& file
		) {
			uint16 scriptIndex = 0;

			while (scriptIndex < scriptList.scriptCount) {
				const ScriptRecord& scriptRecord = scriptList.scriptRecords[scriptIndex];

				if (scriptRecord.scriptTag == STRING_TO_LITTLE_ENDIAN_UINT32("DFLT")) {
					break;
				}

				scriptIndex += 1;
			}

			const ScriptRecord& scriptRecord = scriptList.scriptRecords[scriptIndex];

			stream.seek(tableOffset + gpos.scriptListOffset + scriptRecord.scriptOffset);

			const Script script = decodeScript(stream);

			stream.seek(tableOffset + gpos.scriptListOffset + scriptRecord.scriptOffset + script.defaultLangSysOffset);

			const LangSys defaultLangSys = decodeLangSys(stream);

			uint16 featureIndex;
			uint16 featureIndexIndex = 0;

			while (featureIndexIndex < defaultLangSys.featureIndexCount) {
				featureIndex = defaultLangSys.featureIndices[featureIndexIndex];

				const FeatureRecord& featureRecord = featureList.featureRecords[featureIndex];

				if (featureRecord.featureTag == STRING_TO_LITTLE_ENDIAN_UINT32("kern")) {
					break;
				}

				featureIndexIndex += 1;
			}

			const FeatureRecord& featureRecord = featureList.featureRecords[featureIndex];

			stream.seek(tableOffset + gpos.featureListOffset + featureRecord.featureOffset);

			const Feature feature = decodeFeature(stream);

			for (const uint16 lookupIndex : feature.lookupListIndices) {
				const Offset16 lookupOffset = lookupList.lookupOffsets[lookupIndex];
				const uint64 offset = tableOffset + gpos.lookupListOffset + lookupOffset;

				stream.seek(offset);

				const Lookup lookup = decodeLookup(stream);

				switch (lookup.lookupType) {
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
					default: {
						ENGINE_WARNING("Encountered unsupported TTF lookup type ([]).", lookup.lookupType);

						break;
					}
				}
			}
		}

		void decodeGPOSTable(ByteStream<StreamOperation::READ>& stream, const std::unordered_map<uint32, uint32>& tableOffsetMap, File<FileFormat::TTF>& file) {
			const std::unordered_map<uint32, uint32>::const_iterator tableOffset = tableOffsetMap.find(STRING_TO_LITTLE_ENDIAN_UINT32("GPOS"));

			if (tableOffset == tableOffsetMap.end()) {
				return;
			}

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

			stream.seek(tableOffset->second + gpos.scriptListOffset);

			ScriptList scriptList{};
			scriptList.scriptCount = stream.read<uint16>();
			scriptList.scriptRecords = sequence<ScriptRecord>(scriptList.scriptCount);

			for (ScriptRecord& scriptRecord : scriptList.scriptRecords) {
				scriptRecord.scriptTag = stream.read<Tag>(true);
				scriptRecord.scriptOffset = stream.read<Offset16>();
			}

			stream.seek(tableOffset->second + gpos.lookupListOffset);

			LookupList lookupList{};
			lookupList.lookupCount = stream.read<uint16>();
			lookupList.lookupOffsets = sequence<Offset16>(lookupList.lookupCount);

			for (Offset16& lookupOffset : lookupList.lookupOffsets) {
				lookupOffset = stream.read<Offset16>();
			}

			stream.seek(tableOffset->second + gpos.featureListOffset);

			FeatureList featureList{};
			featureList.featureCount = stream.read<uint16>();
			featureList.featureRecords = sequence<FeatureRecord>(featureList.featureCount);

			for (FeatureRecord& featureRecord : featureList.featureRecords) {
				featureRecord.featureTag = stream.read<Tag>(true);
				featureRecord.featureOffset = stream.read<Offset16>();
			}

			stream.seek(tableOffset->second + gpos.scriptListOffset);

			applyDefaultLangSys(stream, tableOffset->second, gpos, scriptList, lookupList, featureList, file);
		}
	}
}