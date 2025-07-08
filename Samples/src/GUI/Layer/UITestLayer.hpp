#pragma once

namespace ProjectA {
	class UITestLayer : public GUILayer {
		public:
			void registerAssets() override {
				font = loadFont("../ProjectA/src/Asset/Font/test-regular.ttf");

				containerStyle.size = {1280_px, 720_px};
				containerStyle.padding = {8_px, 8_px};
				containerStyle.layoutDirection = HORIZONTAL;
				containerStyle.layoutSpacing = 4_px;

				// containerStyle.color = 0x313131ff;
				containerStyle.color = 0xffffffff;

				buttonStyle.size = {FIT, FIT};
				buttonStyle.padding = {10_px, 8_px};
				// buttonStyle.color = 0x494949ff;
				buttonStyle.color = 0xffffffff;
				buttonStyle.layoutSpacing = 8_px;
				buttonStyle.outlineColor = 0xd9d9d9ff;

				imageStyle.size = {16_px, 16_px};
				imageStyle.color = 0x2c3338ff;
				imageStyle.borderRadius = 8_px;

				textStyle.color = 0x000000ff;
				textStyle.font = font;

				if constexpr (true) {
					// Small
					buttonStyle.outlineSize = 1_px;
					buttonStyle.borderRadius = 5.0f;
					textStyle.fontSize = 16.0f;
				}
				else {
					// Large
					buttonStyle.outlineSize = 5_px;
					buttonStyle.borderRadius = 16.0f;
					textStyle.fontSize = 64.0f;
				}
			}

			void GUI() override {
				GUIComponent(containerStyle) {
					GUIComponent(buttonStyle) {
						GUIText("Button", textStyle);
					}
				}
			}

		private:
			FontID font;
			ContainerStyle containerStyle;
			ContainerStyle buttonStyle;
			ContainerStyle imageStyle;
			TextStyle textStyle;
	};
}