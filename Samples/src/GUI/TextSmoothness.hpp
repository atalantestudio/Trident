// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// TODO: WIP
	class GUITextSmoothnessLayer : public GUILayer {
		public:
			void registerAssets() override {
				font = loadFontAsset("test-regular");
			}

			void render() override {
				Track();

				Text("The quick brown fox jumps over the lazy dog.", {
					.color = 0xc2c2c2ff,
					.font = font,
					.fontSize = 16_px,
				});
			}

		private:
			void Track() {
				Container({
					.size = {300_px, FIT},
					.color = 0xff9800ff,
				}) {
					Thumb();
				}
			}

			void Thumb() {
				float32 offset = 0.0f;

				if (dragging) {
					currentX = getMousePosition()[0];

					offset = currentX;
				}

				Container({
					.size = {40_px, 40_px},
					.padding = {offset, 0.0f},
					.color = 0xde1818ff,
				}) {
					if (IsMouseDown()) {
						dragging = true;
						currentX = getMousePosition()[0];
					}

					if (IsMouseUp()) {
						dragging = false;
					}
				}

				previousX = currentX;
			}

		private:
			FontID font;

			bool dragging = false;
			float32 currentX = 0.0f;
			float32 previousX = 0.0f;
	};
}