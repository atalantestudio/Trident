#pragma once

#include "ProjectA.hpp"

namespace ProjectA {
	class StatisticsLayer : public GUILayer {
		public:
			StatisticsLayer() :
				framesPerSecond(Application::get().getFramesPerSecond())
			{}

			void updateGUI() override {
				PROFILE_SCOPE(FUNC_SIG);

				if (frameIndex == FRAME_DELAY) {
					frameIndex = 0;
					framesPerSecond = Application::get().getFramesPerSecond();
				}

				GUIContainer({212px, EXPAND}, {8px, 8px}, VERTICAL, 8px) {
					GUIContainer({196px, 96px}, {12px, 12px}, VERTICAL, 0px, GRAY) {
						GUIText(32px, "FPS: []", framesPerSecond);
					}

					GUIContainer({196px, 148px}, {12px, 12px}, VERTICAL, 8px, GRAY) {
						for (uint8 i = 0; i < 2; i += 1) {
							GUIQuad({EXPAND, 24px}, LIGHT_GRAY);
						}
					}
				}

				frameIndex += 1;
			}

		private:
			static constexpr uint8 FRAME_DELAY = 165;

			static inline const vec4<float32> GRAY = {0.122f, 0.122f, 0.122f, 1.0f};
			static inline const vec4<float32> LIGHT_GRAY = {0.196f, 0.204f, 0.196f, 1.0f};

		private:
			uint8 frameIndex = 0;
			float64 framesPerSecond;
	};
}