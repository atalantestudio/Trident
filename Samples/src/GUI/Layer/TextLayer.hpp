#pragma once

namespace ProjectA {
	class SampleLayer : public GUILayer {
		public:
			void registerAssets() override {
				containerStyle.size.width = FIT;
				containerStyle.padding = {16_px, 16_px};

				textStyle.color = 0x000000ff;
				textStyle.font = loadFontAsset("test-regular");
				textStyle.fontSize = 28.0f;
			}

			void GUI() override {
				GUIComponent(containerStyle) {
					GUIText("Quá trình mã hóa và.", textStyle);
				}
			}

		private:
			ContainerStyle containerStyle;
			TextStyle textStyle;
	};
}