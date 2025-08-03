#pragma once

#include "../Subprocess/BuildTrueTypeFontSubprocess.hpp"

namespace ProjectA {
	class BuildProcess : public Process {
		public:
			explicit BuildProcess(const sequence<void*>& consoleArguments) {
				const char* projectPath = static_cast<const char*>(consoleArguments[PROJECT_PATH_ARGUMENT_INDEX]);

				std::string fontDirectoryPath = projectPath;
				fontDirectoryPath += "/Asset/Font";

				const std::filesystem::directory_iterator fontDirectoryIterator(fontDirectoryPath);

				for (const std::filesystem::directory_entry& directoryEntry : fontDirectoryIterator) {
					if (directoryEntry.is_directory()) {
						continue;
					}

					BuildTrueTypeFontSubprocess(directoryEntry.path().string());
				}

				BREAKPOINT();
			}

		private:
			static constexpr uint32 PROJECT_PATH_ARGUMENT_INDEX = 1;
	};
}