// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/Filesystem/Reader.hpp"
#include "Engine/SampleGroup.hpp"

namespace Trident {
	class PropertyFile final {
		public:
			static view<char8> getDefaultFileData() {
				return (
					"GraphicsAPI: Vulkan\n"
					"PreferIntegratedGPU: 0\n"
					"WindowWidth: 1280\n"
					"WindowHeight: 720\n"
					"VerticalSync: 1"
				);
			}

		private:
			template<typename T>
			static T parseValueString(view<char8> valueString, view<char8> propertyName) {
				uint32 value = 0;

				for (const char8 character : valueString) {
					value *= 10;
					value += character - '0';
				}

				return static_cast<T>(value);
			}

			template<>
			static GraphicsAPI parseValueString(view<char8> valueString, view<char8> propertyName) {
				if (valueString == "CPU") {
					return GraphicsAPI::CPU;
				}

				if (valueString == "DirectX12") {
					return GraphicsAPI::DX12;
				}

				if (valueString == "OpenGL") {
					return GraphicsAPI::GL;
				}

				if (valueString == "Vulkan") {
					return GraphicsAPI::VK;
				}

				ENGINE_ASSERT(false, "Invalid value \"[]\" for property \"[]\".", valueString, propertyName);

				return static_cast<GraphicsAPI>(0);
			}

			// TODO(debug-only): Only used in the Samples project.
			template<>
			static SampleGroup parseValueString(view<char8> valueString, view<char8> propertyName) {
				if (valueString == "GUI") {
					return SampleGroup::GUI;
				}

				if (valueString == "Physics2D") {
					return SampleGroup::PHYSICS_2D;
				}

				if (valueString == "Process") {
					return SampleGroup::PROCESS;
				}

				if (valueString == "Sample") {
					return SampleGroup::SAMPLE;
				}

				if (valueString == "Sample3D") {
					return SampleGroup::SAMPLE_3D;
				}

				ENGINE_ASSERT(false, "Invalid value \"[]\" for property \"[]\".", valueString, propertyName);

				return static_cast<SampleGroup>(0);
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
				properties->PreferIntegratedGPU = getProperty<bool>("PreferIntegratedGPU");
				properties->WindowWidth = getProperty<uint32>("WindowWidth");
				properties->WindowHeight = getProperty<uint32>("WindowHeight");
				properties->VerticalSync = getProperty<bool>("VerticalSync");
			}

			template<typename T>
			T getProperty(view<char8> propertyName) {
				reader->seekg(std::ios::beg);

				std::string line;
				std::size_t propertyNameOffset;

				while (std::getline(reader->getStream(), line)) {
					propertyNameOffset = line.find(propertyName);

					if (propertyNameOffset == std::string::npos) {
						continue;
					}

					const std::size_t valueStartOffset = line.find(':', propertyNameOffset) + 1;

					ENGINE_ASSERT(valueStartOffset != std::string::npos, "The property file is corrupted.");

					const view<char8> value(&line[valueStartOffset], line.size() - valueStartOffset);
					const view<char8> trimmedValue = trim(value);

					return parseValueString<T>(trimmedValue, propertyName);
				}

				ENGINE_ASSERT(false, "Could not find the value for \"[]\" in the property file.", propertyName);

				return static_cast<T>(-1);
			}

		public:
			std::unordered_map<std::string, std::vector<uint8>> propertyMap;

		private:
			Reader* reader;
			std::streampos size;
	};
}