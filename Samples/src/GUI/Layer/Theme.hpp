#pragma once

#include "Engine/API/GUI/Theme/Default/Button.hpp"

namespace ProjectA {
	class SampleLayer : public GUILayer {
		public:
			void registerAssets() override {
				textStyle.color = 0xffffffff;
				textStyle.font = loadFontAsset("test-regular");
				textStyle.fontSize = 24_px;
			}

			void GUI() override {
				GUIComponent(Button()) {
					GUIText("Button", textStyle);
				}
			}

		private:
			TextStyle textStyle;
	};
}