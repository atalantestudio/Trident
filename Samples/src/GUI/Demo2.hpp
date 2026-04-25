// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUIDemo2Layer : public GUILayer {
		public:
			void render() override {
				ContainerStyle blue{};
				blue.size = {FIT, FIT};
				blue.padding = {8_px, 8_px};
				blue.layoutDirection = VERTICAL;
				blue.layoutSpacing = 8_px;
				blue.color = 0x0a6e89ff;
				blue.debugName = "blue";

				GUIComponent(blue) {
					for (uint32 i = 0; i < 3; i += 1) {
						ContainerStyle pink{};
						pink.size = {FIT, FIT};
						pink.padding = {8_px, 8_px};
						pink.color = 0xfb938fff;
						pink.debugName = "pink";

						ContainerStyle lightBlue{};
						lightBlue.size = {FIT, FIT};
						lightBlue.padding = {8_px, 8_px};
						lightBlue.layoutDirection = HORIZONTAL;
						lightBlue.layoutSpacing = 8_px;
						lightBlue.color = 0x5ecbe4ff;
						lightBlue.debugName = "lightBlue";

						ContainerStyle yellow{};
						yellow.size = {64_px, 64_px};
						yellow.color = 0xfed84dff;
						yellow.debugName = "yellow";

						GUIComponent(pink) {
							GUIComponent(lightBlue) {
								GUIComponent(yellow);
								GUIComponent(yellow);
							}
						}
					}
				}
			}
	};
}