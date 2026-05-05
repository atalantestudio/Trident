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
				RangeControl(300_px, dragging, offset, previousOffset, value);

				ENGINE_DEBUG(value);

				Text("The quick brown fox jumps over the lazy dog.", {
					.color = 0xc2c2c2ff,
					.font = font,
					.fontSize = 16_px,
				});
			}

			void onMouseUpClient(MouseUpEvent& event) override {
				dragging = false;
			}

		private:
			void RangeControl(float32 width, bool& dragging, float32& offset, float32& previousOffset, float32& value) {
				Container({
					.size = {width, 23_px},
					.padding = {7_px, 7_px},
					.color = 0xf5f5f5ff,
					.debugName = "RangeControlContainer",
				}) {
					RangeControlTrack();
					RangeControlThumb(width, dragging, offset, previousOffset, value);
				}
			}

			void RangeControlTrack() {
				Floating({
					.offset = {0_px, 11_px},
					.size = {EXPAND, 1_px},
					.color = 0x535353ff,
					.debugName = "RangeControlTrack",
				});
			}

			void RangeControlThumb(float32 width, bool& dragging, float32& offset, float32& previousOffset, float32& value) {
				if (dragging) {
					const float32 currentOffset = getMousePosition()[0];

					offset += currentOffset - previousOffset;
					previousOffset = currentOffset;

					offset = clamp(offset, 0_px, width);
					value = offset / width;
				}

				Floating({
					.offset = {offset - 7_px, 0.0f},
					.size = {14_px, 23_px},
					.color = 0x5cafe7ff,
					.pointerType = PointerType::INTERACTIVE,
					.debugName = "RangeControlThumb",
				}) {
					if (IsMouseDown()) {
						dragging = true;
						previousOffset = getMousePosition()[0];
					}

					if (IsMouseUp()) {
						dragging = false;
					}
				}
			}

		private:
			FontID font;

			bool dragging = false;
			float32 offset = 7_px;
			float32 previousOffset = 7_px;
			float32 value = 0.0f;
	};
}