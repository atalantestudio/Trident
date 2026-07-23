// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/AssetBuilderLibrary.hpp"
#include "../Subprocess/BuildDefaultFontSubprocess.hpp"
#include "../Subprocess/BuildTrueTypeFontSubprocess.hpp"

#include "Engine/API/Process/Process.hpp"

namespace Trident {
	class BuildFontAssetProcess final : public Process {
		private:
			static constexpr uint32 PROJECT_INPUT_PATH_ARGUMENT_INDEX = 1;
			static constexpr uint32 PROJECT_OUTPUT_PATH_ARGUMENT_INDEX = 2;

		public:
			explicit BuildFontAssetProcess(AssetBuilderLibrary* assetLibrary) :
				assetLibrary(assetLibrary)
			{}

			sint32 main(view<void*> arguments) override {
				const std::filesystem::path rootInputPath = static_cast<const char8*>(arguments[PROJECT_INPUT_PATH_ARGUMENT_INDEX]);
				const std::filesystem::path rootOutputPath = static_cast<const char8*>(arguments[PROJECT_OUTPUT_PATH_ARGUMENT_INDEX]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/";
				const std::filesystem::recursive_directory_iterator fontDirectoryIterator(rootInputPath / "Asset/Font");

				BuildDefaultFontSubprocess _(assetOutputPath, assetLibrary);

				for (const std::filesystem::directory_entry& directoryEntry : fontDirectoryIterator) {
					if (directoryEntry.is_directory()) {
						continue;
					}

					BuildTrueTypeFontSubprocess(directoryEntry, assetOutputPath, assetLibrary);
				}

				ENGINE_INFO("Done building font assets.");

				return 0;
			}

		private:
			AssetBuilderLibrary* assetLibrary;
	};
}