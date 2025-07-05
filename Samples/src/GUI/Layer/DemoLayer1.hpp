#pragma once

namespace ProjectA {
	class DemoLayer1 : public GUILayer {
		public:
			void GUI() override {
				ContainerStyle blue;
				blue.size = {1600_px, FIT};
				blue.padding = {32_px, 32_px};
				blue.layoutDirection = HORIZONTAL;
				blue.layoutSpacing = 32_px;
				blue.color = 0x0a6e89ff;
				blue.radius = 16.0f;
				blue.debugName = "blue";

				GUIComponent(blue) {
					ContainerStyle pink;
					pink.size = {300_px, EXPAND};
					pink.padding = {0_px, 0_px};
					pink.layoutDirection = HORIZONTAL;
					pink.layoutSpacing = 0_px;
					pink.color = 0xfb938fff;
					pink.radius = 16.0f;
					pink.debugName = "pink";

					GUIComponent(pink);

					ContainerStyle yellow;
					yellow.size = {EXPAND, 200_px};
					yellow.padding = {0_px, 0_px};
					yellow.layoutDirection = HORIZONTAL;
					yellow.layoutSpacing = 0_px;
					yellow.color = 0xfed84dff;
					yellow.radius = 16.0f;
					yellow.debugName = "yellow";

					GUIComponent(yellow);

					ContainerStyle lightBlue;
					lightBlue.size = {EXPAND, 300_px};
					lightBlue.padding = {0_px, 0_px};
					lightBlue.layoutDirection = HORIZONTAL;
					lightBlue.layoutSpacing = 0_px;
					lightBlue.color = 0x5ecbe4ff;
					lightBlue.radius = 16.0f;
					lightBlue.debugName = "lightBlue";

					GUIComponent(lightBlue);
				}
			}
	};
}