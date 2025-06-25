#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class EditorLayer : public GUILayer {
		public:
			void registerAssets() override {
				textStyle.color = 0xffffffff;
				textStyle.fontSize = static_cast<float32>(32_px);
			}

			void GUI() override {
				PROFILE_SCOPE(FUNC_SIG);

				/* GUIComponent({EXPAND, 100_px}, {20_px, 20_px}, HORIZONTAL, 10_px, GRAY) {
					for (uint8 i = 0; i < 3; i += 1) {
						GUIComponent({150_px, EXPAND}, {10_px, 10_px}, HORIZONTAL, 0_px, ORANGE) {
							GUIComponent({EXPAND, EXPAND}, LIGHT_GRAY);
						}
					}
				} */

				// GUIImage({512_px, 512_px}, 1);

				const std::string string = Logger::format("FPS: []", Application::getFramesPerSecond());

				// GUIText(string, textStyle);
			}

		private:
			static inline const vec4<float32> GRAY = {0.122f, 0.122f, 0.122f, 1.0f};
			static inline const vec4<float32> LIGHT_GRAY = {0.196f, 0.204f, 0.196f, 1.0f};
			static inline const vec4<float32> ORANGE = {1.0f, 0.2f, 0.0f, 1.0f};

		private:
			TextStyle textStyle;
	};
}