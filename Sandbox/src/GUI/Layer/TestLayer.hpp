#pragma once

namespace ProjectA {
	class TestLayer : public GUILayer {
		public:
			void GUI() override {
				static TextStyle textStyle;
				textStyle.color = {0.0f, 0.0f, 0.0f, 1.0f};
				textStyle.fontSize = 256.0f;

				GUIContainer({EXPAND, EXPAND}, {0px, 0px}, HORIZONTAL, 0px, WHITE) {
					GUIText({"Radio"}, textStyle);
				}
			}

		private:
			static inline const vec4<float32> GRAY = {0.122f, 0.122f, 0.122f, 1.0f};
			static inline const vec4<float32> LIGHT_GRAY = {0.196f, 0.204f, 0.196f, 1.0f};
			static inline const vec4<float32> YELLOW = {1.0f, 1.0f, 0.0f, 1.0f};
			static inline const vec4<float32> WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
	};
}