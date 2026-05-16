// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/Filesystem/Reader.hpp"

namespace Trident {
	template<typename T>
	inline T deserialize(view<char8> text) {
		uint64 value = 0;

		for (const char8 character : text) {
			value *= 10;
			value += character - '0';
		}

		return static_cast<T>(value);
	}

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
					"SampleGroupName: Graphics\n"
					"SampleName: Triangle\n"
					"\n"
					"AssetName: test-regular"
				);
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

						return deserialize<T>(trim(value));
					}

					offset += 1;
					previousOffset = offset;
				}

				ENGINE_ASSERT(false, "Could not find the value for \"[]\" in the property file.", propertyName);

				return static_cast<T>(-1);
			}

		private:
			sequence<char8> data;
	};

	template<>
	inline view<char8> PropertyFile::getProperty(view<char8> propertyName) {
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
}