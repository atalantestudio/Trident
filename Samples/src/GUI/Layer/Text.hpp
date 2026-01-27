// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace ProjectA {
	class SampleLayer : public GUILayer {
		public:
			void registerAssets() override {
				listStyle.size = {FIT, FIT};
				listStyle.padding = {8_px, 0_px};
				listStyle.layoutDirection = VERTICAL;
				listStyle.layoutSpacing = 8_px;
				listStyle.color = 0x000000ff;

				itemStyle.size = {FIT, FIT};
				itemStyle.anchorPoint = AnchorPoint::VCENTER;
				itemStyle.layoutDirection = HORIZONTAL;
				itemStyle.color = 0x000000ff;

				itemLabelStyle.color = 0xa5a5a5ff;
				itemLabelStyle.font = loadFontAsset("test-regular-italic");
				itemLabelStyle.fontSize = 16_px;

				itemTextStyle.color = 0xc2c2c2ff;
				itemTextStyle.font = loadFontAsset("test-regular");
			}

			void GUI() override {
				GUIComponent(listStyle) {
					float32 fontSize = 96_px;

					while (fontSize > 8_px) {
						GUIComponent(itemStyle) {
							itemTextStyle.fontSize = fontSize;

							GUIText(Logger::format("[]px", static_cast<uint32>(fontSize)), itemLabelStyle);
							GUIText("The quick brown fox jumps over the lazy dog.", itemTextStyle);
						}

						fontSize -= 8_px;
					}
				}
			}

		private:
			ContainerStyle listStyle;
			ContainerStyle itemStyle;
			TextStyle itemLabelStyle;
			TextStyle itemTextStyle;
	};
}