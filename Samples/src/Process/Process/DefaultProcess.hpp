#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	struct TIMHeader {
		uint32 _signature = 0x513F8FD2;
		uint32 format;
		uint32 width;
		uint32 height;
	};

	class DefaultProcess : public Process {
		public:
			DefaultProcess() {
				const TrueTypeFont font("../ProjectA/src/Asset/Font/test-regular.ttf");
				const SDFGlyphMap& glyphMap = font.getGlyphMap();

				FileStream fileStream = Filesystem::open2<OpenMode::WRITE>("Asset/Image/test.tim");

				sequence<uint8> file(sizeof(uint32) * 4);

				TIMHeader header;
				header.format = static_cast<uint8>(ImageFormat::R8_UNORM);
				header.width = glyphMap.extent.width;
				header.height = glyphMap.extent.height;

				static constexpr uint32 SIGNATURE = 0x513F8FD2;

				std::copy(static_cast<uint8*>(static_cast<void*>(&header)), static_cast<uint8*>(static_cast<void*>(&header)) + sizeof(TIMHeader), &file[0]);

				fileStream.write(file);
				fileStream.flush();
			}
	};
}