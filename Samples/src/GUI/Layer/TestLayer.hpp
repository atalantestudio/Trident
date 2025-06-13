#pragma once

namespace ProjectA {
	class TestLayer : public GUILayer {
		public:
			TestLayer() {
				imageId = loadImage("Asset/Image/Pepper.png");
			}

			void GUI() override {
				/*static constexpr float32 fontSize = 20.0f;

				static TextStyle whiteTextStyle;
				whiteTextStyle.color = {1.0f, 1.0f, 1.0f, 1.0f};
				whiteTextStyle.fontSize = fontSize;

				static TextStyle yellowTextStyle;
				yellowTextStyle.color = {1.0f, 1.0f, 0.0f, 1.0f};
				yellowTextStyle.fontSize = fontSize;

				TextStyle buttonTextStyle;
				buttonTextStyle.color = {0.0f, 0.0f, 0.0f, 1.0f};
				buttonTextStyle.fontSize = 256.0f;

				static ButtonStyle buttonStyle;
				buttonStyle.textStyle = &buttonTextStyle;*/

				GUIComponent({512, 512}, imageId);

				// GUIContainer({EXPAND, EXPAND}, {0px, 0px}, HORIZONTAL, 0px, LIGHT_GRAY) {
					// GUIText("Pick up ", whiteTextStyle);
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
			ImageID imageId;
	};
}