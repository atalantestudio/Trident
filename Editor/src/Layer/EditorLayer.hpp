#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class EditorLayer : public GUILayer {
		public:
			EditorLayer() {
				textStyle.color = {1.0f, 1.0f, 1.0f, 1.0f};
				textStyle.fontSize = 32px;
			}

			void GUI() override {
				PROFILE_SCOPE(FUNC_SIG);

				GUIContainer({EXPAND, 100px}, {20px, 20px}, HORIZONTAL, 10px, GRAY) {
					for (uint8 i = 0; i < 3; i += 1) {
						GUIContainer({150px, EXPAND}, {10px, 10px}, HORIZONTAL, 0px, ORANGE) {
							GUIComponent({EXPAND, EXPAND}, LIGHT_GRAY);
						}
					}
				}

				GUIImage({512px, 512px}, 1);

				const std::string string = Logger::format("FPS: []", Application::getFramesPerSecond());

				GUIText(string, textStyle);
			}

		private:
			static inline const vec4<float32> GRAY = {0.122f, 0.122f, 0.122f, 1.0f};
			static inline const vec4<float32> LIGHT_GRAY = {0.196f, 0.204f, 0.196f, 1.0f};
			static inline const vec4<float32> ORANGE = {1.0f, 0.2f, 0.0f, 1.0f};

		private:
			TextStyle textStyle;
	};
}