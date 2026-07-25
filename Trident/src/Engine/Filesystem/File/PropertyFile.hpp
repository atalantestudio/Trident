// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAPI.hpp"
#include "Engine/Application/ApplicationProperties.hpp"
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
					"Window.Width: 1280\n"
					"Window.Height: 720\n"
					"Window.Maximized: 1\n"
					"VerticalSync: 1\n"

					// TODO(test)
					// These values are used by the Samples application and are not part of the engine.
					// Remove them when a separate generator is made for the samples.
					"\n"
					"SampleGroupName: Graphics\n"
					"SampleName: Triangle\n"
					"\n"
					"AssetName: test-regular"
				);
			}

		public:
			explicit PropertyFile(view<char8> data) :
				data(data)
			{}

			uint64 getSize() const {
				return data.count();
			}

			void readProperties(ApplicationProperties* properties) {
				properties->GraphicsAPI = getProperty<GraphicsAPI>("GraphicsAPI");
				properties->PreferIntegratedGPU = getProperty<bool>("PreferIntegratedGPU");
				properties->Window.Width = getProperty<uint32>("Window.Width");
				properties->Window.Height = getProperty<uint32>("Window.Height");
				properties->Window.Maximized = getProperty<bool>("Window.Maximized");
				properties->VerticalSync = getProperty<bool>("VerticalSync");
			}

			template<typename T>
			T getProperty(view<char8> propertyName) const {
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
	inline view<char8> PropertyFile::getProperty(view<char8> propertyName) const {
		const view<char8> lines = data;

		index previousOffset = 0;
		index offset = 0;

		while (offset < lines.count()) {
			offset = lines.find('\n', previousOffset);

			const view<char8> line(&lines[previousOffset], offset - previousOffset);

			// TODO(property-file)
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