#pragma once

#include "Engine/base.hpp"
#include "Engine/Event/Event.hpp"

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

			// TODO: Deprecate. The Layer should be created by the application
			// and not before startup.
			// 
			// template<typename T, typename... U>
			// void LayerStack::registerLayer<T>(U&&... arguments);
			virtual void bind() {}
			virtual void unbind() {}

			virtual void update(float32 deltaTime) {}

			// NOTE: There is no dedicated ImGui layer.
			// Every layer has its own ImGui window.
			virtual void renderImGui() {}

			virtual void onEvent(Event& event) {}
	};
}