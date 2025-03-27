#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class DemoGUILayer : public GUILayer {
		public:
			DemoGUILayer();
	};

	class ColoredQuad : public GUIObject {
		public:
			ColoredQuad(const vec2<float32>& position, const vec2<float32>& size, uint32 colorIndex) {
				setColorIndex(colorIndex);

				GUISubcomponent subcomponent;
				subcomponent.offset = position;
				subcomponent.size = size;
				subcomponent.uv = {0.0f, 0.0f};

				subcomponents.emplace_back(subcomponent);
			}
	};

	class TexturedQuad : public GUIObject {
		public:
			TexturedQuad(const vec2<float32>& position, const vec2<float32>& size, uint32 textureIndex) {
				setTextureIndex(textureIndex);

				GUISubcomponent subcomponent;
				subcomponent.offset = position;
				subcomponent.size = size;
				subcomponent.uv = {0.0f, 0.0f};

				subcomponents.emplace_back(subcomponent);
			}
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