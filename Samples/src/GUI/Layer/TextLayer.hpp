#pragma once

namespace ProjectA {
	class SampleLayer : public GUILayer {
		public:
			void registerAssets() override {
				textStyle.color = 0xffffffff;
				textStyle.font = loadFontAsset("test-regular");
				textStyle.fontSize = 24_px * 4;

				imageStyle.size = {1920_px, 1920_px};
				imageStyle.image = loadImageAsset("test-regular");
			}

			void GUI() override {
				GUIComponent(imageStyle);

				// GUIText("Quá trình mã hóa và.\nProjectA::Application()", textStyle);
			}

		private:
			TextStyle textStyle;
			ContainerStyle imageStyle;
	};
}