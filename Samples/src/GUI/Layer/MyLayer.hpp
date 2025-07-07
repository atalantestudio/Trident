#pragma once

namespace ProjectA {
	class MyLayer : public GUILayer {
		public:
			void registerAssets() override {
				fontBold = loadFont("../ProjectA/src/Asset/Font/test-bold.ttf");
				fontRegular = loadFont("../ProjectA/src/Asset/Font/test-regular.ttf");
				fontRegularItalic = loadFont("../ProjectA/src/Asset/Font/test-regular-italic.ttf");
				image = loadImage("Asset/Image/Cameraman.png");

				containerStyle.size = {1280_px, 720_px};
				containerStyle.padding = {12_px, 12_px};
				containerStyle.layoutDirection = VERTICAL;
				containerStyle.layoutSpacing = 16_px;
				containerStyle.color = 0xffffffff;

				imageContainerStyle.size = {FIT, FIT};
				imageContainerStyle.layoutDirection = VERTICAL;
				imageContainerStyle.layoutSpacing = 4_px;

				imageStyle.size = {256_px, 256_px};
				imageStyle.image = image;
				imageStyle.radius = 8.0f;

				titleStyle.color = 0x000000ff;
				titleStyle.font = fontBold;
				titleStyle.fontSize = 24.0f;

				paragraphStyle.color = 0x000000ff;
				paragraphStyle.font = fontRegular;
				paragraphStyle.fontSize = 16.0f;

				legendStyle.color = 0x757575ff;
				legendStyle.font = fontRegularItalic;
				legendStyle.fontSize = 14.0f;
			}

			void GUI() override {
				GUIComponent(containerStyle) {
					GUIText("The Lorem Ipsum", titleStyle);

					GUIText(
						"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"
						"Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\n"
						"Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.\n"
						"Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
						paragraphStyle
					);

					GUIComponent(imageContainerStyle) {
						GUIComponent(imageStyle);

						GUIText("The Cameraman test image.", legendStyle);
					}
				}
			}

		private:
			FontID fontBold;
			FontID fontRegular;
			FontID fontRegularItalic;
			ImageID image;

			ContainerStyle containerStyle;
			ContainerStyle imageContainerStyle;
			ContainerStyle imageStyle;

			TextStyle titleStyle;
			TextStyle paragraphStyle;
			TextStyle legendStyle;
	};
}