#pragma once

#include "ProjectA.hpp"

#include "PNGParsertest.hpp"

namespace ProjectA {
	class PNGTestLayer : public Layer {
		public:
			PNGTestLayer() : Layer("PNGTest") {
				//parsetest(L"assets/textures/checkerboard.png");
				//PNGFileLoader::load("assets/textures/checkerboard.png");

				const uint64 size = 4;
				const uint8 data[size] = {0b11101101, 0b10100110, 0b01001101, 0b10101011};

				BitStream stream(&data[0], size);

				stream.jump(3);
				uint8 readOrPeek = stream.read<uint8>(8);
				stream.jump(8);
			}

			virtual ~PNGTestLayer() = default;

			void bind() override {}
			void unbind() override {}

			void update(const float32 deltaTime) override {}

			void renderImGui() override {}

			void onEvent(Event& event) override {}
	};
}