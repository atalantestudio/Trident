// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUIButtonTestLayer : public GUILayer {
		public:
			void registerAssets() override {
				font = loadFontAsset("test-regular");
			}

			void GUI() override {
				bool hovered = false;

				GUIComponent({
					.size = {200_px, 100_px},
					.color = 0xff980ff,
				}) {
					hovered = IsHovered();
				}

				if (hovered) {
					GUIText("Hovered", {
						.color = 0xffffffff,
						.font = font,
						.fontSize = 30_px,
					});
				}
			}

		private:
			FontID font;
	};
}