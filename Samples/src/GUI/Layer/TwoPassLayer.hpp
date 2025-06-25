#pragma once

namespace ProjectA {
	class TwoPassLayer : public GUILayer {
		public:
			void GUI() override {
				ContainerStyle blue;
				blue.size = {FIT, FIT};
				blue.padding = {0px, 0px};
				blue.layoutDirection = HORIZONTAL;
				blue.layoutSpacing = 0px;
				blue.color = 0x0a6e8975;

				GUIContainer(blue) {
					ContainerStyle pink;
					pink.size = {300px, 300px};
					pink.padding = {0px, 0px};
					pink.layoutDirection = HORIZONTAL;
					pink.layoutSpacing = 0px;
					pink.color = 0xfb938f75;

					GUIContainer(pink);

					ContainerStyle yellow;
					yellow.size = {350px, 200px};
					yellow.padding = {0px, 0px};
					yellow.layoutDirection = HORIZONTAL;
					yellow.layoutSpacing = 0px;
					yellow.color = 0xfed84d75;

					GUIContainer(yellow);
				}
			}
	};
}