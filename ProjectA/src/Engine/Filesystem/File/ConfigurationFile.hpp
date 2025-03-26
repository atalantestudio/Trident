#pragma once

#include "Engine/Filesystem/File/File.hpp"

namespace ProjectA {
	struct ConfigurationFile : public File {
		std::unordered_map<std::string, chain<uint8>> propertyMap;

		template<typename T>
		T get(const std::string& propertyName) const {
			const std::string valueString = get<std::string>(propertyName);

			return static_cast<T>(std::stoi(valueString));
		}

		template<>
		std::string get(const std::string& propertyName) const {
			const auto property = propertyMap.find(propertyName);

			ENGINE_ASSERT(property != propertyMap.end(), "Failed to locate property \"[]\" in configuration property map.", propertyName);

			const chain<uint8>& value = property->second;

			return {value.begin(), value.end()};
		}
	};
}