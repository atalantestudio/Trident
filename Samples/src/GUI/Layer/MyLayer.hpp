#pragma once

namespace ProjectA {
	class MyLayer : public GUILayer {
		public:
			void registerAssets() override {
				radioGroteskBoldFont = loadFont("../ProjectA/src/Asset/Font/test-bold.ttf");
				radioGroteskRegularFont = loadFont("../ProjectA/src/Asset/Font/test-regular.ttf");
				radioGroteskRegularItalicFont = loadFont("../ProjectA/src/Asset/Font/test-regular-italic.ttf");
				image = loadImage("Asset/Image/Cameraman.png");

				titleStyle.color = {0.0f, 0.0f, 0.0f, 1.0f};
				titleStyle.font = radioGroteskBoldFont;
				titleStyle.fontSize = 24.0f;

				paragraphStyle.color = {0.0f, 0.0f, 0.0f, 1.0f};
				paragraphStyle.font = radioGroteskRegularFont;
				paragraphStyle.fontSize = 16.0f;

				legendStyle.color = {0.3f, 0.3f, 0.3f, 1.0f};
				legendStyle.font = radioGroteskRegularItalicFont;
				legendStyle.fontSize = 14.0f;

				buttonStyle.padding = {0px, 0px};
				buttonStyle.backgroundColor = {1.0f, 0.2f, 0.0f, 1.0f};
				buttonStyle.radius = 10.0f;
				buttonStyle.textStyle = &titleStyle;
			}

			void GUI() override {
				GUIContainer({EXPAND, EXPAND}, {12px, 12px}, VERTICAL, 0px, white) {
					GUIText("The Lorem Ipsum", titleStyle);

					GUISpacing(16px);

					GUIText(
						"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"
						"Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\n"
						"Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.\n"
						"Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
						paragraphStyle
					);

					GUISpacing(16px);

					GUIImage({256px, 256px}, image, 8px);

					GUISpacing(4px);

					GUIText("The Cameraman test image.", legendStyle);
				}
			}

		private:
			static inline const vec4<float32> white = {1.0f, 1.0f, 1.0f, 1.0f};

		private:
			FontID radioGroteskBoldFont;
			FontID radioGroteskRegularFont;
			FontID radioGroteskRegularItalicFont;
			ImageID image;

			TextStyle titleStyle;
			TextStyle paragraphStyle;
			TextStyle legendStyle;
			ButtonStyle buttonStyle;
	};
}