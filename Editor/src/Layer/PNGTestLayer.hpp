#pragma once

#include "ProjectA.hpp"

#include "PNGParsertest.hpp"

namespace ProjectA {
	class PNGTestLayer : public Layer {
		public:
			PNGTestLayer() : Layer("PNGTest") {
				//parsetest(L"assets/textures/checkerboard.png");
				PNGFileLoader::load("assets/textures/checkerboard.png");
			}

			virtual ~PNGTestLayer() = default;

			void bind() override {}
			void unbind() override {}

			void update(const float32 deltaTime) override {}

			void renderImGui() override {}

			void onEvent(Event& event) override {}
	};
}