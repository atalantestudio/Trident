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

				GUIContainer({172px, 54px}, {12px, 12px}, HORIZONTAL, 0px, {0.0f, 0.0f, 0.0f, 1.0f}) {
					GUIText("FPS: []", framesPerSecond);
				}

				frameIndex += 1;
			}

		private:
			static constexpr uint8 FRAME_DELAY = 165;

		private:
			uint8 frameIndex = 0;
			float64 framesPerSecond;
	};
}