// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/Filesystem/Reader.hpp"

namespace Trident {
	class PropertyFile final {
		public:
			static view<char8> getDefaultFileData() {
				return (
					"GraphicsAPI: OpenGL\n"
					"PreferIntegratedGPU: 0\n"
					"WindowWidth: 1280\n"
					"WindowHeight: 720\n"
					"VerticalSync: 1\n"
					"\n"
					"SampleGroupName: Sample\n"
					"SampleName: Triangle\n"
					"\n"
					"AssetName: test-regular"
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

		public:
			explicit PropertyFile(const sequence<char8>& data) :
				data(data)
			{}

			uint64 getSize() const {
				return data.count();
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
				const view<char8> lines = data;

				index previousOffset = 0;
				index offset = 0;

				while (offset < lines.count()) {
					offset = lines.find('\n', previousOffset);

					const view<char8> line(&lines[previousOffset], offset - previousOffset);
					const index propertyNameOffset = line.find(propertyName, 0);

					if (propertyNameOffset < line.count()) {
						const index valueStartOffset = line.find(':', propertyNameOffset) + 1;

						ENGINE_ASSERT(valueStartOffset < line.count(), "The property file is corrupted.");

						const view<char8> value(&line[valueStartOffset], line.count() - valueStartOffset);

						return parseValueString<T>(trim(value), propertyName);
					}

					offset += 1;
					previousOffset = offset;
				}

				ENGINE_ASSERT(false, "Could not find the value for \"[]\" in the property file.", propertyName);

				return static_cast<T>(-1);
			}

			template<>
			view<char8> getProperty(view<char8> propertyName) {
				const view<char8> lines = data;

				index previousOffset = 0;
				index offset = 0;

				while (offset < lines.count()) {
					offset = lines.find('\n', previousOffset);

					const view<char8> line(&lines[previousOffset], offset - previousOffset);

					// TODO
					// A false positive is returned if the line contains a word that matches the start of the property name,
					// (e.g. "SampleGroup" vs. "Sample").
					const index propertyNameOffset = line.find(propertyName, 0);

					if (propertyNameOffset < line.count()) {
						const index valueStartOffset = line.find(':', propertyNameOffset) + 1;

						ENGINE_ASSERT(valueStartOffset < line.count(), "The property file is corrupted.");

						const view<char8> value(&line[valueStartOffset], line.count() - valueStartOffset);

						return trim(value);
					}

					offset += 1;
					previousOffset = offset;
				}

				ENGINE_ASSERT(false, "Could not find the value for \"[]\" in the property file.", propertyName);

				return "";
			}

		private:
			sequence<char8> data;
	};
}