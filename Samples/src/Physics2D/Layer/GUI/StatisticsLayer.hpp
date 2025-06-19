#pragma once

namespace ProjectA {
	class StatisticsLayer : public GUILayer {
		public:
			void registerAssets() override {
				font = loadFont("../ProjectA/src/Asset/Font/test-regular.ttf");

				textStyle.color = {1.0f, 1.0f, 1.0f, 1.0f};
				textStyle.font = font;
				textStyle.fontSize = 32.0f;

				framesPerSecond = Application::getFramesPerSecond();
			}

			void GUI() override {
				PROFILE_SCOPE(FUNC_SIG);

				if (Application::getFrameIndex() % FRAME_DELAY == 0) {
					framesPerSecond = Application::getFramesPerSecond();
				}

				GUIContainer({212px, EXPAND}, {8px, 8px}, VERTICAL, 8px) {
					GUIContainer({196px, 96px}, {12px, 12px}, VERTICAL, 0px, gray) {
						const std::string string = Logger::format("FPS: []", framesPerSecond);

						GUIText(string, textStyle);
					}

					GUIContainer({196px, 148px}, {12px, 12px}, VERTICAL, 8px, gray) {
						for (uint8 i = 0; i < 2; i += 1) {
							GUIComponent({EXPAND, 24px}, lightGray);
						}
					}
				}
			}

		private:
			static constexpr uint32 FRAME_DELAY = 165;

			static inline const vec4<float32> gray = {0.122f, 0.122f, 0.122f, 1.0f};
			static inline const vec4<float32> lightGray = {0.196f, 0.204f, 0.196f, 1.0f};

		private:
			FontID font;
			TextStyle textStyle;
			float64 framesPerSecond;
	};
}