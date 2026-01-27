#pragma once

namespace Trident {
	class StatisticsLayer : public GUILayer {
		private:
			static constexpr uint32 FRAME_DELAY = 165;

		public:
			void registerAssets() override {
				textStyle.color = 0xffffffff;
				textStyle.font = loadFontAsset("test-regular");
				textStyle.fontSize = 18.0f;

				containerStyle.size = {FIT, FIT};
				containerStyle.padding = {8.0_px, 8.0_px};
				containerStyle.layoutDirection = VERTICAL;
				containerStyle.color = 0x00000000;

				panelStyle.size = {FIT, FIT};
				panelStyle.padding = {4.0_px, 4.0_px};
				panelStyle.layoutDirection = VERTICAL;
				panelStyle.color = 0x22222275;

				framesPerSecond = getApplication()->getFramesPerSecond();
			}

			void GUI() override {
				if (getApplication()->getFrameIndex() % FRAME_DELAY == 0) {
					framesPerSecond = getApplication()->getFramesPerSecond();
				}

				GUIComponent(containerStyle) {
					GUIComponent(panelStyle) {
						GUIText(Logger::format("FPS: []", framesPerSecond), textStyle);
					}
				}
			}

		private:
			TextStyle textStyle;
			ContainerStyle containerStyle;
			ContainerStyle panelStyle;
			float64 framesPerSecond;
	};
}