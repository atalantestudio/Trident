#pragma once

namespace ProjectA {
	class TwoPassLayer : public GUILayer {
		public:
			void GUI() override {
				ContainerStyle blue;
				blue.size = {160_px, FIT};
				blue.padding = {4_px, 4_px};
				blue.layoutDirection = HORIZONTAL;
				blue.layoutSpacing = 4_px;
				blue.color = 0x0a6e89ff;

				GUIContainer(blue) {
					ContainerStyle pink;
					pink.size = {30_px, 30_px};
					pink.padding = {0_px, 0_px};
					pink.layoutDirection = HORIZONTAL;
					pink.layoutSpacing = 0_px;
					pink.color = 0xfb938fff;

					GUIContainer(pink);

					ContainerStyle yellow;
					yellow.size = {EXPAND, 30_px};
					yellow.padding = {0_px, 0_px};
					yellow.layoutDirection = HORIZONTAL;
					yellow.layoutSpacing = 0_px;
					yellow.color = 0xfed84dff;

					GUIContainer(yellow);

					ContainerStyle lightBlue;
					lightBlue.size = {30_px, 30_px};
					lightBlue.padding = {0_px, 0_px};
					lightBlue.layoutDirection = HORIZONTAL;
					lightBlue.layoutSpacing = 0_px;
					lightBlue.color = 0x5ecbe4ff;

					GUIContainer(lightBlue);
				}
			}
	};
}