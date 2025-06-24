#pragma once

namespace ProjectA {
	class TwoPassLayer : public GUILayer {
		public:
			void GUI() override {
				ContainerStyle blue;
				blue.fillMode = FIT;
				blue.size = {0px, 0px};
				// blue.size = {960px, 540px};
				blue.padding = {0px, 0px};
				blue.layoutDirection = HORIZONTAL;
				blue.layoutSpacing = 0px;
				blue.color = 0x0a6e8975;

				GUIContainer(blue) {
					ContainerStyle red;
					red.fillMode = FIXED;
					red.size = {300px, 300px};
					red.padding = {0px, 0px};
					red.layoutDirection = HORIZONTAL;
					red.layoutSpacing = 0px;
					red.color = 0xfb938f75;

					GUIContainer(red);

					ContainerStyle green;
					green.fillMode = FIXED;
					green.size = {350px, 200px};
					green.padding = {0px, 0px};
					green.layoutDirection = HORIZONTAL;
					green.layoutSpacing = 0px;
					green.color = 0xfed84d75;

					GUIContainer(green);
				}
			}
	};
}