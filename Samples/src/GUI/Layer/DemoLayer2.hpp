#pragma once

namespace ProjectA {
	class DemoLayer2 : public GUILayer {
		public:
			void registerAssets() override {
				font = loadFont("../ProjectA/src/Asset/Font/test-regular.ttf");
			}

			void GUI() override {
				ContainerStyle blue;
				blue.size = {1000_px, 500_px};
				blue.padding = {32_px, 32_px};
				blue.anchorPoint = AnchorPoint::LEFT | AnchorPoint::TOP;
				blue.layoutSpacing = 32_px;
				blue.color = 0x0a6e89ff;
				blue.debugName = "blue";

				GUIComponent(blue) {
					TextStyle text1;
					text1.color = 0xf99394ff;
					text1.font = font;
					text1.fontSize = 64.0f;
					text1.debugName = "text1";

					GUIText("One Two Three Four Five", text1);

					ContainerStyle yellow;
					yellow.size = {300_px, 300_px};
					yellow.color = 0xfed84dff;
					yellow.debugName = "yellow";

					GUIComponent(yellow);

					ContainerStyle lightBlue;
					lightBlue.size = {FIT, FIT};
					lightBlue.padding = {32_px, 32_px};
					lightBlue.color = 0x5ecbe4ff;
					lightBlue.debugName = "lightBlue";

					GUIComponent(lightBlue) {
						TextStyle text2;
						text2.color = 0xb45bd0ff;
						text2.font = font;
						text2.fontSize = 64.0f;
						text2.debugName = "text2";

						GUIText("Five Six Seven Eight Nine Ten", text2);
					}
				}
			}

		private:
			FontID font;
	};
}