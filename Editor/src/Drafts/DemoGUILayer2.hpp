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
				size = {300.0f, 0.0f};
				fillMode = FillMode::EXPAND_VERTICAL;
				layoutDistribution = LayoutDistribution::VERTICAL;
				margin = {6.0f, 6.0f, 6.0f, 6.0f};

				append<GUIText>("Lorem ipsum dolor sit amet.");
			}
	};

	class PersonListItem : public GUIObject {
		public:
			PersonListItem(const Person& person) {
				margin = {6.0f, 6.0f, 6.0f, 6.0f};
				fillMode = FillMode::EXPAND_HORIZONTAL;
				spacing = {6.0f, 6.0f};
				color = Color::GREY;
				layoutDirection = LayoutDirection::RIGHT;

				append<GUIImage>(person.iconUrl);
			}
	};
}