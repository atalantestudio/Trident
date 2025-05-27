#pragma once

namespace ProjectA {
	class TestLayer : public GUILayer {
		public:
			void GUI() override {
				static constexpr float32 fontSize = 20.0f;

				static TextStyle whiteTextStyle;
				whiteTextStyle.color = {1.0f, 1.0f, 1.0f, 1.0f};
				whiteTextStyle.fontSize = fontSize;
				whiteTextStyle.outlineWidth = 0.0f;
				whiteTextStyle.outlineColor = {0.0f, 0.0f, 0.0f, 1.0f};

				static TextStyle yellowTextStyle;
				yellowTextStyle.color = {1.0f, 1.0f, 0.0f, 1.0f};
				yellowTextStyle.fontSize = fontSize;
				yellowTextStyle.outlineWidth = 0.0f;
				yellowTextStyle.outlineColor = {0.0f, 0.0f, 0.0f, 1.0f};

				GUIContainer({EXPAND, EXPAND}, {0px, 0px}, HORIZONTAL, 0px, LIGHT_GRAY) {
					GUIText({"Pick up "}, whiteTextStyle);
					GUIText({"Alarm Clock"}, yellowTextStyle);
				}
			}

		private:
			static inline const vec4<float32> GRAY = {0.122f, 0.122f, 0.122f, 1.0f};
			static inline const vec4<float32> LIGHT_GRAY = {0.196f, 0.204f, 0.196f, 1.0f};
			static inline const vec4<float32> YELLOW = {1.0f, 1.0f, 0.0f, 1.0f};
			static inline const vec4<float32> WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
	};
}