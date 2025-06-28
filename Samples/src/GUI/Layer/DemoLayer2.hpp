#pragma once

namespace ProjectA {
	class DemoLayer2 : public GUILayer {
		public:
			void registerAssets() override {
				font = loadFont("../ProjectA/src/Asset/Font/test-regular.ttf");
			}

			void GUI() override {
				ContainerStyle blue;
				blue.size = {1000_px, FIT};
				blue.padding = {32_px, 32_px};
				blue.layoutSpacing = 32_px;
				blue.color = 0x0a6e89ff;

				GUIComponent(blue) {
					TextStyle text1;
					text1.color = 0xffffffff;
					text1.font = font;
					text1.fontSize = 32.0f;

					GUIText("One Two Three Four", text1);

					ContainerStyle yellow;
					yellow.size = {300_px, 300_px};
					yellow.color = 0xfed84dff;

					GUIComponent(yellow);

					ContainerStyle lightBlue;
					lightBlue.size = {FIT, FIT};
					lightBlue.padding = {32_px, 32_px};
					lightBlue.color = 0x0a6e89ff;

					GUIComponent(lightBlue) {
						TextStyle text2;
						text2.color = 0xffffffff;
						text2.font = font;
						text2.fontSize = 32.0f;

						GUIText("Five Six Seven Eight Nine Ten", text2);
					}
				}
			}

		private:
			FontID font;
	};
}