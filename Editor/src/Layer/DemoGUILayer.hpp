#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class DemoGUILayer : public GUILayer {
		public:
			DemoGUILayer();
	};

	struct Person {
		uint32 id = 0;
		std::string name;
		std::string iconUrl;
	};

	class PersonListItem : public GUIObject {
		public:
			PersonListItem(const Person& person) {
				setDefaultSize({0.0f, 50.0f});
				//setMargin({6.0f, 6.0f});
				setFillMode(FillMode::EXPAND_HORIZONTAL);
				//setSpacing({6.0f, 6.0f});
				//setBackground(Color::GREY);
				//setLayoutDistribution(LayoutDistribution::HORIZONTAL);
				setColor(vec4<float32>(50.0f, 52.0f, 50.0f, 255.0f) / 255.0f);

				//append<ColoredQuad>(1);
				//210, 210, 210
			}
	};

	class PersonListWindow : public GUIObject {
		public:
			PersonListWindow() {
				// A window of 300px * (max height).
				setDefaultSize({300.0f, 0.0f});
				setFillMode(FillMode::EXPAND_VERTICAL);
				//setLayoutDistribution(LayoutDistribution::VERTICAL);
				setMargin({10.0f, 10.0f});
				setColor(vec4<float32>(31.0f, 31.0f, 31.0f, 255.0f) / 255.0f);

				Person person;
				person.id = 1;
				person.name = "John Doe";

				append<PersonListItem>(person);
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