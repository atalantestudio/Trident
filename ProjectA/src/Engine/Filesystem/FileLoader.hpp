#pragma once

namespace ProjectA {
	class FileLoader {
		public:
			static std::string load(const std::string& path);

			static chain<uint8> loadChain(const std::string& path);
	};
}