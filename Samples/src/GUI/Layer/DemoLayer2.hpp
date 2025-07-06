#pragma once

namespace ProjectA {
	class DemoLayer2 : public GUILayer {
		public:
			void GUI() override {
				ContainerStyle blue;
				blue.size = {FIT, FIT};
				blue.padding = {8_px, 8_px};
				blue.layoutDirection = VERTICAL;
				blue.layoutSpacing = 8_px;
				blue.color = 0x0a6e89ff;
				blue.radius = 4.0f;
				blue.debugName = "blue";

				GUIComponent(blue) {
					for (uint32 i = 0; i < 3; i += 1) {
						ContainerStyle pink;
						pink.size = {FIT, FIT};
						pink.padding = {8_px, 8_px};
						pink.color = 0xfb938fff;
						pink.layoutDirection = VERTICAL;
						pink.radius = 4.0f;
						pink.debugName = "pink";

						ContainerStyle yellow;
						yellow.size = {64_px, 64_px};
						yellow.color = 0xfed84dff;
						yellow.radius = 4.0f;
						yellow.debugName = "yellow";

						GUIComponent(pink) {
							GUIComponent(yellow);
						}
					}
				}
			}
	};
}