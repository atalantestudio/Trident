#pragma once

#include "ProjectA/Event/KeyEvent.hpp"
#include "ProjectA/Event/MouseEvent.hpp"
#include "ProjectA/Event/WindowEvent.hpp"
#include "ProjectA/Layer.hpp"

namespace ProjectA {
	class API ImGuiLayer : public Layer {
		private:
			float deltaTime = 0.0f;

		public:
			ImGuiLayer();
			~ImGuiLayer() = default;

			virtual void bind() override;
			virtual void unbind() override;
			void begin();
			virtual void renderImGui() override;
			void end();
	};
}