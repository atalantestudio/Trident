#pragma once

#include "Engine/Filesystem/Reader.hpp"

namespace ProjectA {
	class PropertyFile final {
		public:
			static std::string getDefaultFileData() {
				return (
					"GraphicsAPI: 2\n"
					"WindowWidth: 1280\n"
					"WindowHeight: 720\n"
					"VSync: true"
				);
			}

		private:
			template<typename T>
			static T parseValueString(const std::string& valueString, std::string_view propertyName) {
				uint32 value = 0;

				for (const char character : valueString) {
					value *= 10;
					value += character - '0';
				}

				return static_cast<T>(value);
			}

			template<>
			static bool parseValueString(const std::string& valueString, std::string_view propertyName) {
				if (valueString == "true") {
					return true;
				}

				if (valueString == "false") {
					return false;
				}

				ENGINE_ERROR("Invalid value \"[]\" for property \"[]\".", valueString, propertyName);

				return false;
			}

		public:
			explicit PropertyFile(Reader& reader) :
				reader(&reader)
			{
				size = reader.tellg();

				reader.seekg(std::ios::beg);
			}

			uint64 getSize() const {
				return size;
			}

			void readProperties(ApplicationProperties* properties) {
				properties->GraphicsAPI = getProperty<GraphicsAPI>("GraphicsAPI");
				properties->WindowWidth = getProperty<uint32>("WindowWidth");
				properties->WindowHeight = getProperty<uint32>("WindowHeight");
				properties->VSync = getProperty<bool>("VSync");
			}

		private:
			template<typename T>
			T getProperty(std::string_view propertyName) {
				reader->seekg(std::ios::beg);

				std::string line;
				std::size_t propertyNameOffset;

				while (std::getline(reader->getStream(), line)) {
					propertyNameOffset = line.find(propertyName);

					if (propertyNameOffset == std::string::npos) {
						continue;
					}

					const std::size_t valueStringStartOffset = line.find(':', propertyNameOffset) + 1;

					ENGINE_ASSERT(valueStringStartOffset != std::string::npos, "The property file is corrupted.");

					const std::string valueString = trim(line.substr(valueStringStartOffset, line.size()));

					return parseValueString<T>(valueString, propertyName);
				}

				ENGINE_ERROR("Could not find the value for \"[]\" in the property file.", propertyName);

				return static_cast<T>(-1);
			}

		public:
			std::unordered_map<std::string, std::vector<uint8>> propertyMap;

		private:
			Reader* reader;
			std::streampos size;
	};
}