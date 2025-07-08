#pragma once

namespace ProjectA {
	class UITestLayer : public GUILayer {
		public:
			void registerAssets() override {
				font = loadFont("../ProjectA/src/Asset/Font/test-regular.ttf");

				containerStyle.size = {1280_px, 720_px};
				containerStyle.padding = {8_px, 8_px};
				containerStyle.layoutDirection = VERTICAL;
				containerStyle.layoutSpacing = 4_px;
				containerStyle.color = 0x313131ff;

				buttonStyle.size = {FIT, FIT};
				buttonStyle.padding = {4_px, 2_px};
				buttonStyle.color = 0x494949ff;
				buttonStyle.borderRadius = 3.0f;

				textStyle.color = 0xffffffff;
				textStyle.font = font;
				textStyle.fontSize = 16.0f;
			}

			void GUI() override {
				GUIComponent(containerStyle) {
					GUIComponent(buttonStyle) {
						GUIText("Load Asset", textStyle);
					}

					GUIComponent(buttonStyle) {
						GUIText("Generate Mipmaps", textStyle);
					}
				}
			}

		private:
			FontID font;
			ContainerStyle containerStyle;
			ContainerStyle buttonStyle;
			TextStyle textStyle;
	};
}