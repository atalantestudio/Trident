#pragma once

#include "Engine/Process/Process.hpp"

#include "Subprocess/BuildDefaultFontSubprocess.hpp"
#include "Subprocess/BuildTrueTypeFontSubprocess.hpp"

namespace ProjectA {
	class BuildFontProcess : public Process {
		private:
			static constexpr uint32 PROJECT_INPUT_PATH_ARGUMENT_INDEX = 1;
			static constexpr uint32 PROJECT_OUTPUT_PATH_ARGUMENT_INDEX = 2;

		public:
			sint32 main(const sequence<void*>& arguments) override {
				const std::string rootInputPath = static_cast<const char8*>(arguments[PROJECT_INPUT_PATH_ARGUMENT_INDEX]);
				const std::string rootOutputPath = static_cast<const char8*>(arguments[PROJECT_OUTPUT_PATH_ARGUMENT_INDEX]);
				const std::string assetOutputPath = rootOutputPath + "/Asset/";

				const std::filesystem::recursive_directory_iterator fontDirectoryIterator(rootInputPath + "/Asset/Font");

				BuildDefaultFontSubprocess defaultFontSubprocess(assetOutputPath);

				for (const std::filesystem::directory_entry& directoryEntry : fontDirectoryIterator) {
					if (directoryEntry.is_directory()) {
						continue;
					}

					const BuildTrueTypeFontSubprocess subprocess(directoryEntry, assetOutputPath);
				}

				ENGINE_INFO("Done building font assets.");

				return 0;
			}
	};
}