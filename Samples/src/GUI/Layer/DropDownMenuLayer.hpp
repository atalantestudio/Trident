#pragma once

namespace ProjectA {
	class DropDownMenuLayer : public GUILayer {
		public:
			void registerAssets() override {
				font = loadFontAsset("test-regular");
			}

			void GUI() override {
				ContainerStyle menu{};
				menu.size = {FIT, FIT};
				menu.padding = {8_px, 8_px};
				menu.layoutDirection = VERTICAL;
				menu.layoutSpacing = 8_px;
				menu.color = 0xa675a2ff;
				menu.borderRadius = 4_px;
				menu.debugName = "menu";

				GUIComponent(menu) {
					for (uint8 itemIndex = 0; itemIndex < 5; itemIndex += 1) {
						const char8* label = itemLabels[itemIndex];

						ContainerStyle item{};
						item.size = {EXPAND, 40_px};
						item.minimumSize.y = 40_px;
						item.padding = {8_px, 8_px};
						item.anchorPoint = AnchorPoint::LEFT | AnchorPoint::VCENTER;
						item.layoutSpacing = 8_px;
						item.color = 0xbd95bcff;
						item.borderRadius = 4_px;
						item.debugName = "item";

						GUIComponent(item) {
							ContainerStyle labelContainer{};
							labelContainer.size = {EXPAND, FIT};
							labelContainer.color = 0x00000000;
							labelContainer.debugName = "labelContainer";

							GUIComponent(labelContainer) {
								TextStyle itemLabel{};
								itemLabel.color = 0xffffffff;
								itemLabel.font = font;
								itemLabel.fontSize = 32;

								GUIText(std::string(label), itemLabel);
							}

							ContainerStyle itemImage{};
							itemImage.size = {32_px, 32_px};
							itemImage.color = 0xff9800ff;
							itemImage.debugName = "itemImage";

							GUIComponent(itemImage);
						}
					}
				}
			}

		private:
			static inline constexpr char8* itemLabels[5] = {
				"Copy",
				"Paste",
				"Delete",
				"Layer",
				"Comment",
			};

			FontID font;
	};
}