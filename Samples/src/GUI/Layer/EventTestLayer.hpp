// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/Theme/Default/Button.hpp"

namespace Trident {
	class SampleLayer : public GUILayer {
		public:
			void registerAssets() override {
				windowStyle.size = {EXPAND, EXPAND};
				windowStyle.padding = {8_px, 8_px};
				windowStyle.layoutSpacing = 8_px;

				textStyle.color = 0xa5a5a5ff;
				textStyle.font = loadFontAsset("test-bold");
				textStyle.fontSize = 20_px;
			}

			void GUI() override {
				GUIComponent(windowStyle) {
					GUIComponentClass(button, [&]() {
						GUIText("Click me, I'm a button.", textStyle);
					});
				}
			}

		private:
			ContainerStyle windowStyle;
			Button button;
			TextStyle textStyle;
	};
}