#pragma once

#include "Engine/Filesystem/File/File.hpp"

namespace ProjectA {
	class PropertyFile final : public File {
		public:
			std::unordered_map<std::string, chain<uint8>> propertyMap;

			PropertyFile(std::string_view path) :
				stream(path.data(), std::ios::in | std::ios::out | std::ios::ate)
			{
				const std::filesystem::path absolutePath = std::filesystem::absolute(path);

				if (!stream.is_open()) {
					ENGINE_FATAL(ENOENT, "Could not open property file [].", absolutePath);

					return;
				}

				name = absolutePath.filename().string();
				this->absolutePath = absolutePath.string();
				size = stream.tellg();
			}

			~PropertyFile() {
				stream.close();
			}

			template<typename T>
			T getProperty(std::string_view propertyName) {
				stream.seekg(0, std::ios::beg);

				std::string line;
				std::size_t propertyNameOffset;

				while (std::getline(stream, line)) {
					propertyNameOffset = line.find(propertyName);

					if (propertyNameOffset == std::string::npos) {
						continue;
					}

					const std::size_t valueStringStartOffset = line.find(':', propertyNameOffset) + 1;

					ENGINE_ASSERT(valueStringStartOffset != std::string::npos, "The property file is corrupted.");

					const std::string valueString = line.substr(valueStringStartOffset, line.size());

					return static_cast<T>(std::stoi(valueString));
				}

				ENGINE_ERROR("Could not find the value for \"[]\" in the property file.", propertyName);

				return static_cast<T>(-1);
			}

			// TODO: Implement real-time property file update.
			/*template<typename T>
			void setProperty(std::string_view propertyName, T value) {
				stream.seekg(0, std::ios::beg);

				std::string line;
				std::size_t propertyNameOffset;
				std::streampos lineStartOffset = 0;

				while (std::getline(stream, line)) {
					propertyNameOffset = line.find(propertyName);

					if (propertyNameOffset == std::string::npos) {
						lineStartOffset = stream.tellg();

						continue;
					}

					const std::size_t valueStringStartOffset = line.find(':', propertyNameOffset) + 1;

					ENGINE_ASSERT(valueStringStartOffset != std::string::npos, "The property file is corrupted.");

					const std::string valueString = ' ' + std::to_string(value);

					stream.seekg(lineStartOffset);
					stream.ignore(valueStringStartOffset);
					stream.write(valueString.data(), valueString.size());
					stream.flush();

					return;
				}

				ENGINE_ERROR("Could not find the value for \"[]\" in the property file.", propertyName);
			}*/

		private:
			std::fstream stream;
	};
}