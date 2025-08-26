#pragma once

namespace ProjectA {
	class TextLayer : public GUILayer {
		public:
			void registerAssets() override {
				textStyle.color = 0x000000ff;
				textStyle.font = loadFontAsset("test-regular");
				textStyle.fontSize = 64.0f;
			}

			void GUI() override {
				GUIText("Quá trình mã hóa và.", textStyle);
			}

		private:
			TextStyle textStyle;
	};
}