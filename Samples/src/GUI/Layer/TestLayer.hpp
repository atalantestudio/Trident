#pragma once

namespace ProjectA {
	class TestLayer : public GUILayer {
		public:
			TestLayer() {
				// imageId = loadImage("Asset/Image/Pepper.png");
				fontId = loadFont("../ProjectA/src/Asset/Font/test.ttf");

				whiteTextStyle.color = {1.0f, 1.0f, 1.0f, 1.0f};
				whiteTextStyle.fontSize = 64.0f;
			}

			void GUI() override {
				GUIText("PP Radio Grotesk", whiteTextStyle);
				GUIComponent({2048, 2048}, 1);

				// GUIContainer({EXPAND, EXPAND}, {0px, 0px}, HORIZONTAL, 0px, LIGHT_GRAY) {
					// GUIText("Alarm Clock", yellowTextStyle);

					// if (GUIButton("Test Button", buttonStyle)) {
						// ENGINE_DEBUG("Button hovered");
					// }
				// }
			}

		private:
			static inline const vec4<float32> GRAY = {0.122f, 0.122f, 0.122f, 1.0f};
			static inline const vec4<float32> LIGHT_GRAY = {0.196f, 0.204f, 0.196f, 1.0f};
			static inline const vec4<float32> YELLOW = {1.0f, 1.0f, 0.0f, 1.0f};
			static inline const vec4<float32> WHITE = {1.0f, 1.0f, 1.0f, 1.0f};

		private:
			TextStyle whiteTextStyle;

			ImageID imageId;
			FontID fontId;
	};
}