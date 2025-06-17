#pragma once

namespace ProjectA {
	class WindowLayer : public GUILayer {
		public:
			WindowLayer() {
				radioGroteskRegularFont = loadFont("../ProjectA/src/Asset/Font/test-regular.ttf");

				textStyle.color = white;
				textStyle.font = radioGroteskRegularFont;
				textStyle.fontSize = 16.0f;

				buttonStyle.padding = {4px, 2px};
				buttonStyle.backgroundColor = lightGray;
				buttonStyle.radius = 3.0f;
				buttonStyle.textStyle = &textStyle;
			}

			void GUI() override {
				GUIContainer({EXPAND, EXPAND}, {8px, 8px}, VERTICAL, 4px, gray) {
					GUIButton("Load Asset", buttonStyle);
					GUIButton("Generate Mipmaps", buttonStyle);
				}
			}

		private:
			static inline const vec4<float32> gray = {0.122f, 0.122f, 0.122f, 1.0f};
			static inline const vec4<float32> lightGray = {0.196f, 0.204f, 0.196f, 1.0f};
			static inline const vec4<float32> white = {1.0f, 1.0f, 1.0f, 1.0f};

		private:
			FontID radioGroteskRegularFont;
			TextStyle textStyle;
			ButtonStyle buttonStyle;
	};
}