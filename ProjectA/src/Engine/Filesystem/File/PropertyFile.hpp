#pragma once

#include "Engine/Filesystem/File/File.hpp"

namespace ProjectA {
	class PropertyFile : public File {
		public:
			std::unordered_map<std::string, chain<uint8>> propertyMap;

			PropertyFile() = default;

			PropertyFile(const std::string& path) {
				const std::filesystem::path absolutePath = std::filesystem::absolute(path);
				std::fstream stream(path);

				if (!stream.is_open()) {
					ENGINE_FATAL(ENOENT, "Could not open property file [].", absolutePath);

					return;
				}

				name = absolutePath.filename().string();
				this->absolutePath = absolutePath.string();

				stream.seekg(0, std::ios::end);
				size = stream.tellg();
				stream.seekg(0, std::ios::beg);

				rawData.resize(size);
				stream.read(static_cast<char*>(static_cast<void*>(rawData.data())), size);

				/*const std::string data(rawData.begin(), rawData.end());

				std::size_t beginOffset = 0;
				std::size_t endOffset;

				const uint32 lineIterationCap = static_cast<uint32>(size);

				for (uint32 lineIndex = 0; lineIndex < lineIterationCap; lineIndex += 1) {
					endOffset = data.find("\r\n", beginOffset);

					const std::string line = data.substr(beginOffset, endOffset - beginOffset);

					const std::size_t colonOffset = line.find(':');

					ENGINE_ASSERT(colonOffset != std::string::npos, "Configuration file is malformed at line [].", lineIndex);

					const std::string propertyName = line.substr(0, colonOffset);
					const std::string valueString = trim(line.substr(colonOffset + 1));
					const chain<uint8> value(valueString.begin(), valueString.end());	

					propertyMap.insert(std::make_pair(propertyName, value));

					if (endOffset == std::string::npos) {
						break;
					}

					beginOffset = endOffset + 2;
				}*/
			}

			template<typename T>
			T getProperty(const std::string& propertyName) const {
				const std::string valueString = get<std::string>(propertyName);

				return static_cast<T>(std::stoi(valueString));
			}

			template<>
			std::string getProperty(const std::string& propertyName) const {
				const auto property = propertyMap.find(propertyName);

				ENGINE_ASSERT(property != propertyMap.end(), "Failed to locate property \"[]\" in the property map.", propertyName);

				const chain<uint8>& value = property->second;

				return {value.begin(), value.end()};
			}
	};
}