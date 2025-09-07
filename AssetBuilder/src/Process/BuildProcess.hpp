#pragma once

#include "../Subprocess/BuildDefaultFontSubprocess.hpp"
#include "../Subprocess/BuildTrueTypeFontSubprocess.hpp"

namespace ProjectA {
	class BuildProcess : public Process {
		public:
			explicit BuildProcess(const sequence<void*>& consoleArguments) {
				const std::string rootInputPath = static_cast<const char8*>(consoleArguments[PROJECT_INPUT_PATH_ARGUMENT_INDEX]);
				const std::string rootOutputPath = static_cast<const char8*>(consoleArguments[PROJECT_OUTPUT_PATH_ARGUMENT_INDEX]);

				const std::string assetOutputPath = rootOutputPath + "/Asset/";

				const std::filesystem::directory_iterator fontDirectoryIterator(rootInputPath + "/Asset/Font");

				BuildDefaultFontSubprocess defaultFontSubprocess(assetOutputPath);

				for (const std::filesystem::directory_entry& directoryEntry : fontDirectoryIterator) {
					if (directoryEntry.is_directory()) {
						continue;
					}

					const BuildTrueTypeFontSubprocess subprocess(directoryEntry, assetOutputPath);
				}

				ENGINE_INFO("Done building assets.");
			}

		private:
			static constexpr uint32 PROJECT_INPUT_PATH_ARGUMENT_INDEX = 1;
			static constexpr uint32 PROJECT_OUTPUT_PATH_ARGUMENT_INDEX = 2;
	};
}