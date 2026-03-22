// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/Theme/Default/Button.hpp"

namespace Trident {
	class GUIEventTestLayer : public GUILayer {
		private:
			static constexpr uint32 LIST_ITEM_COUNT = 5;

		public:
			void registerAssets() override {
				listContainerStyle.size = {EXPAND, EXPAND};
				listContainerStyle.layoutDirection = VERTICAL;
				listContainerStyle.color = 0xffffffff;

				listItemTextStyle.color = 0x000000ff;
				listItemTextStyle.font = loadFontAsset("test-regular");
				listItemTextStyle.fontSize = 30_px;

				listItemStyle.size = {200_px, 100_px};
				listItemStyle.color = 0xff9800ff;
			}

			void GUI() override {
				GUIComponent(listContainerStyle) {
					uint32 listItemIndex = 0;

					while (listItemIndex < LIST_ITEM_COUNT) {
						GUIComponent(listItemStyle) {
							if (isCurrentComponentHovered()) {
								GUIText(Logger::format("Hovered []", listItemIndex), listItemTextStyle);
							}
							else {
								GUIText(Logger::format("Not hovered []", listItemIndex), listItemTextStyle);
							}
						}

						listItemIndex += 1;
					}
				}
			}

		private:
			ContainerStyle listContainerStyle;
			ContainerStyle listItemStyle;
			TextStyle listItemTextStyle;
	};
}