#pragma once

#include "Engine/Process/Process.hpp"

#include "../Subprocess/EncodeGeometryAssetSubprocess.hpp"

namespace Trident {
	class BuildGeometryProcess : public Process {
		private:
			static constexpr uint32 PROJECT_INPUT_PATH_ARGUMENT_INDEX = 1;
			static constexpr uint32 PROJECT_OUTPUT_PATH_ARGUMENT_INDEX = 2;

		public:
			sint32 main(const sequence<void*>& arguments) override {
				const std::filesystem::path rootInputPath = pointer_cast<const char8*>(arguments[PROJECT_INPUT_PATH_ARGUMENT_INDEX]);
				const std::filesystem::path rootOutputPath = pointer_cast<const char8*>(arguments[PROJECT_OUTPUT_PATH_ARGUMENT_INDEX]);
				const std::filesystem::path assetOutputPath = rootOutputPath / "Asset/";
				const std::filesystem::recursive_directory_iterator geometryDirectoryIterator(rootInputPath / "Asset/Geometry");

				for (const std::filesystem::directory_entry& directoryEntry : geometryDirectoryIterator) {
					if (directoryEntry.is_directory()) {
						continue;
					}

					EncodeGeometryAssetSubprocess(directoryEntry, assetOutputPath);
				}

				ENGINE_INFO("Done building geometry assets.");

				return 0;
			}
	};
}