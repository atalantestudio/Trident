#pragma once

#include "ProjectA/base.hpp"
#include "ProjectA/Event/Event.hpp"

namespace ProjectA {
	class Layer {
		protected:
			// TODO: Don't include in release (and profile as well?)
			std::string name;

		public:
			Layer(const std::string& name = "Layer");

			virtual ~Layer() = default;

			const std::string& getName() const {
				return name;
			}

			virtual void bind() {}
			virtual void unbind() {}

			virtual void update(float deltaTime) {}

			/// There is not a dedicated ImGui layer. Every layer can have its own ImGui window.
			virtual void renderImGui() {}

			virtual void onEvent(Event& event) {}
	};
}