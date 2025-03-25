#pragma once

#include "Engine/Event/KeyEvent.hpp"
#include "Engine/Event/MouseEvent.hpp"
#include "Engine/Event/WindowEvent.hpp"
#include "Engine/Layer/Layer.hpp"

namespace ProjectA {
	class ImGuiLayer : public Layer {
		public:
			bool blockEvents = true;

		public:
			ImGuiLayer();
			~ImGuiLayer() = default;

			virtual void bind() override;
			virtual void unbind() override;

			void begin();
			void end();

			virtual void onEvent(Event& event) override;

		private:
			float32 deltaTime = 0.0f;
	};
}