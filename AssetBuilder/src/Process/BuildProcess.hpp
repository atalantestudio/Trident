#pragma once

#include "BuildBitmapFontProcess.hpp"

namespace ProjectA {
	class BuildProcess : public Process {
		public:
			explicit BuildProcess() {
				// 1. Find all JSON files in Asset/Font.
				// const std::filesystem::path sandbox{"Asset/Font"};

				// 2. For each file:
				// for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{sandbox})

				BREAKPOINT();
			}
	};
}