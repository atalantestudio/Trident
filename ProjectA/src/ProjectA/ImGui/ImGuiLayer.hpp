#pragma once

#include "ProjectA/Layer.hpp"

namespace ProjectA {
	class API ImGuiLayer : public Layer {
		private:
			float deltaTime = 0.0f;

		public:
			ImGuiLayer();
			~ImGuiLayer();

			void attach();
			void detach();
			void update();
			void listen(Event& event);
	};
}