// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/GUILayer.hpp"

namespace ProjectB {
	using namespace Trident;

	class UIDefaultTheme : public GUILayer {
		public:
			void registerAssets() override {
				font = loadFontAsset("test-regular");

				clearColor = primaryColor;
			}

		protected:
			void updateThemeColors(bool darkMode) {
				if (darkMode) {
					primaryColor = 0x0a0a0aff;
					secondaryColor = 0x3a3a3aff;
					backgroundHoverColor = 0x1a1a1aff;
					borderFocusColor = 0xc2c2c2ff;
					textColor = 0xa5a5a5ff;
					textHoverColor = 0xc2c2c2ff;
				}
				else {
					primaryColor = 0xf0f0f0ff;
					secondaryColor = 0x989898ff;
					backgroundHoverColor = 0xd2d2d2ff;
					borderFocusColor = 0x3a3a3aff;
					textColor = 0x0a0a0aff;
					textHoverColor = 0x535353ff;
				}

				clearColor = primaryColor;
			}

		protected:
			Color primaryColor = 0x00000000;
			Color secondaryColor = 0x00000000;
			Color backgroundHoverColor = 0x00000000;
			Color borderFocusColor = 0x00000000;
			Color textColor = 0x00000000;
			Color textHoverColor = 0x00000000;
			FontID font;
	};
}