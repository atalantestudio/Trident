#pragma once

#include "ProjectA/Event/KeyEvent.hpp"
#include "ProjectA/Event/MouseEvent.hpp"
#include "ProjectA/Event/WindowEvent.hpp"
#include "ProjectA/Layer.hpp"

namespace ProjectA {
	class ImGuiLayer : public Layer {
		private:
			float deltaTime = 0.0f;

		public:
			ImGuiLayer();
			~ImGuiLayer() = default;

			virtual void bind() override;
			virtual void unbind() override;
			void begin();
			void end();
	};
}