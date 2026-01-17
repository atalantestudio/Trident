#pragma once

namespace ProjectA {
	class SampleLayer : public GUILayer {
		public:
			void registerAssets() override {
				windowContainerStyle.size = {EXPAND, EXPAND};
				windowContainerStyle.padding = {16_px, 16_px};
				windowContainerStyle.layoutDirection = VERTICAL;
				windowContainerStyle.layoutSpacing = 16_px;

				listContainerStyle.size = {EXPAND, FIT};
				listContainerStyle.padding = {1_px, 1_px};
				listContainerStyle.layoutDirection = VERTICAL;
				listContainerStyle.layoutSpacing = 1_px;
				listContainerStyle.color = 0x3a3a3aff;

				listItemContainerStyle.size = {EXPAND, FIT};
				listItemContainerStyle.padding = {8_px, 8_px};
				listItemContainerStyle.layoutDirection = VERTICAL;
				listItemContainerStyle.layoutSpacing = 8_px;
				listItemContainerStyle.color = 0x0a0a0aff;

				listItemTitleStyle.color = 0xa5a5a5ff;
				listItemTitleStyle.font = loadFontAsset("test-bold");
				listItemTitleStyle.fontSize = 22_px;

				listItemDescriptionStyle.color = 0xa5a5a5ff;
				listItemDescriptionStyle.font = loadFontAsset("test-regular");
				listItemDescriptionStyle.fontSize = 16_px;
			}

			void GUI() override {
				GUIComponent(windowContainerStyle) {
					GUIComponent(listContainerStyle) {
						// Option 1
						GUIComponent(listItemContainerStyle) {
							GUIText("Option 1", listItemTitleStyle);
							GUIText("Description for option 1", listItemDescriptionStyle);
						}

						// Option 1
						GUIComponent(listItemContainerStyle) {
							GUIText("Option 2", listItemTitleStyle);
							GUIText("Description for option 2", listItemDescriptionStyle);
						}
					}
				}
			}

		private:
			ContainerStyle windowContainerStyle;
			ContainerStyle listContainerStyle;
			ContainerStyle listItemContainerStyle;
			TextStyle listItemTitleStyle;
			TextStyle listItemDescriptionStyle;
	};
}