#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class DemoScriptEntity : public ScriptEntity {
		public:
			void onConstruct() {
				// TODO: Allow retrieving other components from this scriptable entity, e.g.:
				//const GUIComponent& component = getComponent<GUIComponent>();
				//ENGINE_DEBUG("Color index = []", component.colorIndex);
			}
	};

	class GUILayer : public Layer {
		private:
			GUIRenderer renderer;

		public:
			GUILayer();

			virtual ~GUILayer() = default;

			void bind() override;
			void unbind() override;

			void update(float32 deltaTime) override;

			void renderImGui() override;

			void onEvent(Event& event) override;

			bool onWindowResize(WindowResizeEvent& event);
	};
}