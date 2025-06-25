#pragma once

namespace ProjectA {
	class TwoPassLayer : public GUILayer {
		public:
			void GUI() override {
				ContainerStyle blue;
				blue.size = {1600_px, FIT};
				blue.padding = {32_px, 32_px};
				blue.layoutDirection = HORIZONTAL;
				blue.layoutSpacing = 32_px;
				blue.color = 0x0a6e89ff;
				blue.radius = 16.0f;

				GUIContainer(blue) {
					ContainerStyle pink;
					pink.size = {300_px, 300_px};
					pink.padding = {0_px, 0_px};
					pink.layoutDirection = HORIZONTAL;
					pink.layoutSpacing = 0_px;
					pink.color = 0xfb938fff;
					pink.radius = 16.0f;

					GUIContainer(pink);

					ContainerStyle yellow;
					yellow.size = {EXPAND, 300_px};
					yellow.padding = {0_px, 0_px};
					yellow.layoutDirection = HORIZONTAL;
					yellow.layoutSpacing = 0_px;
					yellow.color = 0xfed84dff;
					yellow.radius = 16.0f;

					GUIContainer(yellow);

					ContainerStyle lightBlue;
					lightBlue.size = {300_px, 300_px};
					lightBlue.padding = {0_px, 0_px};
					lightBlue.layoutDirection = HORIZONTAL;
					lightBlue.layoutSpacing = 0_px;
					lightBlue.color = 0x5ecbe4ff;
					lightBlue.radius = 16.0f;

					GUIContainer(lightBlue);
				}
			}
	};
}