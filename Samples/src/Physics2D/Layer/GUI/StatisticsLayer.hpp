#pragma once

namespace ProjectA {
	class StatisticsLayer : public GUILayer {
		public:
			void registerAssets() override {
				font = loadFontAsset("test-regular");

				textStyle.color = 0x000000ff;
				textStyle.font = font;
				textStyle.fontSize = 16.0f;

				framesPerSecond = Application::getFramesPerSecond();
			}

			void GUI() override {
				if (Application::getFrameIndex() % FRAME_DELAY == 0) {
					framesPerSecond = Application::getFramesPerSecond();
				}

				GUIText(Logger::format("FPS: []", framesPerSecond), textStyle);

				/* GUIComponent({212_px, EXPAND}, {8_px, 8_px}, VERTICAL, 8_px) {
					GUIComponent({196_px, 96_px}, {12_px, 12_px}, VERTICAL, 0_px, gray) {
						const std::string string = Logger::format("FPS: []", framesPerSecond);

						GUIText(string, textStyle);
					}

					GUIComponent({196_px, 148_px}, {12_px, 12_px}, VERTICAL, 8_px, gray) {
						for (uint8 i = 0; i < 2; i += 1) {
							GUIComponent({EXPAND, 24_px}, lightGray);
						}
					}
				} */
			}

		private:
			static constexpr uint32 FRAME_DELAY = 165;
			// static inline const vec4<float32> GRAY = {0.122f, 0.122f, 0.122f, 1.0f};
			// static inline const vec4<float32> LIGHT_GRAY = {0.196f, 0.204f, 0.196f, 1.0f};

		private:
			FontID font;
			TextStyle textStyle;
			float64 framesPerSecond;
	};
}