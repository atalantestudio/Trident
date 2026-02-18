// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUIUTF8Layer : public GUILayer {
		public:
			void registerAssets() override {
				textStyle.color = 0xffffffff;
				textStyle.font = loadFontAsset("test-regular");
				textStyle.fontSize = 96_px;
			}

			void GUI() override {
				GUIText("Quá trình mã hóa vàâ.", textStyle);
			}

		private:
			TextStyle textStyle;
	};
}