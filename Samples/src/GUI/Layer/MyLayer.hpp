#pragma once

namespace ProjectA {
	class MyLayer : public GUILayer {
		public:
			MyLayer() {
				radioGroteskBoldFontId = loadFont("../ProjectA/src/Asset/Font/test-bold.ttf");
				radioGroteskRegularFontId = loadFont("../ProjectA/src/Asset/Font/test-regular.ttf");

				titleStyle.color = {0.0f, 0.0f, 0.0f, 1.0f};
				titleStyle.font = radioGroteskBoldFontId;
				titleStyle.fontSize = 24.0f;

				paragraphStyle.color = {0.0f, 0.0f, 0.0f, 1.0f};
				paragraphStyle.font = radioGroteskRegularFontId;
				paragraphStyle.fontSize = 16.0f;
			}

			void GUI() override {
				GUIContainer({EXPAND, EXPAND}, {12px, 12px}, VERTICAL, 12px, white) {
					GUIText("The Lorem Ipsum", titleStyle);
					GUIText(
						"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"
						"Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\n"
						"Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.\n"
						"Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
						paragraphStyle
					);
				}
			}

		private:
			static inline const vec4<float32> white = {1.0f, 1.0f, 1.0f, 1.0f};

		private:
			FontID radioGroteskBoldFontId;
			FontID radioGroteskRegularFontId;

			TextStyle titleStyle;
			TextStyle paragraphStyle;
	};
}