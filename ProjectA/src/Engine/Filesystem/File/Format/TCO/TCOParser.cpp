#include "pch.hpp"

#include "Engine/Filesystem/File/ConfigurationFile.hpp"
#include "Engine/Filesystem/Filesystem.hpp"

namespace ProjectA {
	template<>
	ConfigurationFile Filesystem::load(const std::string& path) {
		const File file = load<File>(path);

		ConfigurationFile configurationFile;
		configurationFile.name = file.name;
		configurationFile.absolutePath = file.absolutePath;
		configurationFile.size = file.size;
		configurationFile.rawData = file.rawData;

		const std::string data(file.rawData.begin(), file.rawData.end());

		std::size_t beginOffset = 0;
		std::size_t endOffset;

		const uint32 lineIterationCap = static_cast<uint32>(file.size);

		for (uint32 lineIndex = 0; lineIndex < lineIterationCap; lineIndex += 1) {
			endOffset = data.find("\r\n", beginOffset);

			const std::string line = data.substr(beginOffset, endOffset - beginOffset);

			const std::size_t colonOffset = line.find(':');

			ENGINE_ASSERT(colonOffset != std::string::npos, "Configuration file is malformed at line [].", lineIndex);

			const std::string propertyName = line.substr(0, colonOffset);
			const std::string valueString = trim(line.substr(colonOffset + 1));
			const chain<uint8> value(valueString.begin(), valueString.end());	

			configurationFile.propertyMap.insert(std::make_pair(propertyName, value));

			if (endOffset == std::string::npos) {
				break;
			}

			beginOffset = endOffset + 2;
		}

		return configurationFile;
	}
}