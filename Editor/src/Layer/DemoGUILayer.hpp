#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class DemoGUILayer : public GUILayer {
		public:
			DemoGUILayer();

			virtual ~DemoGUILayer() = default;

			void update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) override;
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