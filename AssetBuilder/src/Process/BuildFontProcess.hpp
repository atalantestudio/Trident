// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Process/Process.hpp"

#include "Subprocess/BuildDefaultFontSubprocess.hpp"
#include "Subprocess/BuildTrueTypeFontSubprocess.hpp"

namespace Trident {
	class BuildFontProcess : public Process {
		private:
			static constexpr uint32 PROJECT_INPUT_PATH_ARGUMENT_INDEX = 1;
			static constexpr uint32 PROJECT_OUTPUT_PATH_ARGUMENT_INDEX = 2;

		public:
			sint32 main(view<void*> arguments) override {
				const std::filesystem::path rootInputPath = pointer_cast<const char8*>(arguments[PROJECT_INPUT_PATH_ARGUMENT_INDEX]);
				const std::filesystem::path rootOutputPath = pointer_cast<const char8*>(arguments[PROJECT_OUTPUT_PATH_ARGUMENT_INDEX]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/";
				const std::filesystem::recursive_directory_iterator fontDirectoryIterator(rootInputPath / "Asset/Font");

				BuildDefaultFontSubprocess defaultFontSubprocess(assetOutputPath);

				for (const std::filesystem::directory_entry& directoryEntry : fontDirectoryIterator) {
					if (directoryEntry.is_directory()) {
						continue;
					}

					BuildTrueTypeFontSubprocess(directoryEntry, assetOutputPath);
				}

				ENGINE_INFO("Done building font assets.");

				return 0;
			}
	};
}