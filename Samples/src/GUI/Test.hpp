// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUITestLayer : public GUILayer {
		public:
			void registerAssets() override {
				font = loadFontAsset("test-regular");
			}

			void render() override {
				Container({
					.size = {FIT, FIT},
					.color = 0xff9800ff,
				}) {
					Text("Hello world", {
						.color = 0xde1818ff,
						.font = font,
						.fontSize = 128_px,
					});
				}
			}

		private:
			FontID font;
	};
}