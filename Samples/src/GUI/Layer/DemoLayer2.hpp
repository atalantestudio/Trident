#pragma once

namespace ProjectA {
	class DemoLayer2 : public GUILayer {
		public:
			void registerAssets() override {
				font = loadFont("../ProjectA/src/Asset/Font/test-regular.ttf");
			}

			void GUI() override {
				ContainerStyle blue;
				blue.size = {1200_px, FIT};
				blue.padding = {32_px, 32_px};
				blue.layoutDirection = HORIZONTAL;
				blue.layoutSpacing = 32_px;
				blue.color = 0x0a6e89ff;
				blue.radius = 0.0f;

				GUIComponent(blue) {
					TextStyle text;
					text.color = 0xffffffff;
					text.font = font;
					text.fontSize = 42.0f;

					GUIText(
						"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. "
						"Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. "
						"Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. "
						"Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
						text
					);
				}
			}

		private:
			FontID font;
	};
}