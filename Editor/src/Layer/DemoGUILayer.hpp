#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class DemoGUILayer : public GUILayer {
		public:
			DemoGUILayer();

			virtual ~DemoGUILayer() = default;

			void bind() override;
			void unbind() override;

			void update(float32 deltaTime) override;

			void onEvent(Event& event) override;

			bool onWindowResize(WindowResizeEvent& event);
	};

	class DemoScriptEntity : public ScriptEntity {
		public:
			void onConstruct() {
				// TODO: Allow retrieving other components from this scriptable entity, e.g.:
				//const GUIComponent& component = getComponent<GUIComponent>();
				//ENGINE_DEBUG("Color index = []", component.colorIndex);
			}
	};
}