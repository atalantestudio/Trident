#pragma once

namespace ProjectA {
	struct Person {
		uint32 id;
		std::string name;
		std::string iconUrl;
	};

	class Window : public GUIObject {
		public:
			Window() {
				// A window of 300px * (max height).
				setDefaultWidth(300.0f);
				setFillMode(FillMode::FILL_VERTICAL);
				setLayoutDistribution(LayoutDistribution::TO_BOTTOM);
				setMargin({6.0f, 6.0f, 6.0f, 6.0f});

				append<GUIText>("Lorem ipsum dolor sit amet.");
			}
	};

	class PersonListItem : public GUIObject {
		public:
			PersonListItem(const Person& person) {
				setMargin({6.0f, 6.0f, 6.0f, 6.0f});
				setFillMode(FillMode::FILL_VERTICAL);
				setSpacing({6.0f, 6.0f});
				setBackground(Color::GREY);
				setLayoutDirection(RIGHT);

				append<GUIImage>(person.iconUrl);
			}
	};
}