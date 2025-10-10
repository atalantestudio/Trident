#pragma once

#include "Engine/Process/Process.hpp"

#include "../Subprocess/EncodeGeometryAssetSubprocess.hpp"

namespace ProjectA {
	class BuildGeometryProcess : public Process {
		private:
			static constexpr uint32 PROJECT_INPUT_PATH_ARGUMENT_INDEX = 1;
			static constexpr uint32 PROJECT_OUTPUT_PATH_ARGUMENT_INDEX = 2;

		public:
			explicit BuildGeometryProcess(const sequence<void*>& consoleArguments) {
				const std::string rootInputPath = static_cast<const char8*>(consoleArguments[PROJECT_INPUT_PATH_ARGUMENT_INDEX]);
				const std::string rootOutputPath = static_cast<const char8*>(consoleArguments[PROJECT_OUTPUT_PATH_ARGUMENT_INDEX]);
				const std::string assetOutputPath = rootOutputPath + "/Asset/";

				const std::filesystem::recursive_directory_iterator geometryDirectoryIterator(rootInputPath + "/Asset/Geometry");

				for (const std::filesystem::directory_entry& directoryEntry : geometryDirectoryIterator) {
					if (directoryEntry.is_directory()) {
						continue;
					}

					EncodeGeometryAssetSubprocess(directoryEntry, assetOutputPath);
				}

				ENGINE_INFO("Done building geometry assets.");
			}
	};
}