#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class PNGTestLayer : public Layer {
		public:
			PNGTestLayer() : Layer("PNGTest") {
				PNGFileLoader::load2("assets/textures/checkerboard.png");

				//chain<uint8> buffer = FileLoader::loadChain("assets/textures/checkerboard.png");
				//BitStream stream(buffer.data(), buffer.size());

				//static constexpr uint8 signature[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

				/*for (uint8 i = 0; i < 8; i += 1) {
					const uint8 s = signature[i];
					const uint8 read = stream.read<uint8>(8);

					if (read != s) {
						throw std::runtime_error("Failed to verify file signature.");
					}
				}*/

				//const uint64 size = 2;
				//const uint8 data[size] = {0b10001001, 0b01010000};
				//BitStream stream(&data[0], size);
				//const uint8 value = stream.peek<uint8>(8);
			}

			virtual ~PNGTestLayer() = default;

			void bind() override {}
			void unbind() override {}

			void update(const float32 deltaTime) override {}

			void renderImGui() override {}

			void onEvent(Event& event) override {}
	};
}