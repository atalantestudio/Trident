// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/Theme/Default/Button.hpp"

namespace Trident {
	class SampleLayer : public GUILayer {
		public:
			void registerAssets() override {
				listContainerStyle.size = {FIT, FIT};
				listContainerStyle.padding = {8_px, 8_px};
				listContainerStyle.layoutDirection = VERTICAL;
				listContainerStyle.layoutSpacing = -1_px;
				listContainerStyle.color = 0x00000000;

				listItemTitleStyle.color = 0xa5a5a5ff;
				listItemTitleStyle.font = loadFontAsset("test-bold");
				listItemTitleStyle.fontSize = 20.592_px * 1.1f;

				listItemDescriptionStyle.color = 0xa5a5a5ff;
				listItemDescriptionStyle.font = loadFontAsset("test-regular");
				listItemDescriptionStyle.fontSize = 17.6_px * 1.1f;

				listItems = sequence<Button>(LIST_ITEM_COUNT);
			}

			void GUI() override {
				GUIComponent(listContainerStyle) {
					uint32 listItemIndex = 0;

					while (listItemIndex < LIST_ITEM_COUNT) {
						GUIComponentClass(listItems[listItemIndex], [&]() {
							GUIText(Logger::format("Option []", listItemIndex), listItemTitleStyle);
							GUIText(Logger::format("Description for option []", listItemIndex), listItemDescriptionStyle);
						});

						listItemIndex += 1;
					}
				}
			}

		private:
			static constexpr uint32 LIST_ITEM_COUNT = 5;

			sequence<Button> listItems;
			ContainerStyle listContainerStyle;
			TextStyle listItemTitleStyle;
			TextStyle listItemDescriptionStyle;
	};
}