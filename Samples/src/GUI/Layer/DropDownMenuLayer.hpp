#pragma once

namespace ProjectA {
	class DropDownMenuLayer : public GUILayer {
		public:
			void registerAssets() override {
				font = loadFont("../ProjectA/src/Asset/Font/test-regular.ttf");
			}

			void GUI() override {
				ContainerStyle menu;
				menu.size = {FIT, FIT};
				menu.layoutDirection = VERTICAL;
				menu.color = 0xa675a2ff;
				menu.radius = 4_px;

				GUIComponent(menu) {
					for (const std::string& label : itemLabels) {
						ContainerStyle item;
						item.size = {FIT, FIT};
						item.color = 0xbd95bcff;
						item.radius = 4_px;
						item.debugName = label;

						GUIComponent(item) {
							TextStyle itemTextStyle;
							itemTextStyle.color = 0xffffffff;
							itemTextStyle.font = font;
							itemTextStyle.fontSize = 32;

							GUIText(label, itemTextStyle);

							/* ContainerStyle itemImage;
							itemImage.size = {32_px, 32_px};
							itemImage.image = 0;

							GUIComponent(itemImage); */
						}
					}
				}
			}

		private:
			static inline const std::array<std::string, 5> itemLabels = {"Copy", "Paste", "Delete", "Layer", "Comment"};

			FontID font;
	};
}