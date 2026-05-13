// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUITextSmoothnessLayer : public GUILayer {
		public:
			void registerAssets() override {
				fontBold = loadFontAsset("test-bold");
				fontBoldItalic = loadFontAsset("test-bold-italic");
				fontRegular = loadFontAsset("test-regular");
				fontRegularItalic = loadFontAsset("test-regular");
			}

			void render() override {
				Container({
					.size = {EXPAND, 720_px},
					.layoutDirection = LayoutDirection::VERTICAL,
					// .color = 0x0a0a0aff,
					.color = 0xffffffff,
				}) {
					Container({
						.size = {FIT, FIT},
						.color = 0xff9800ff,
					}) {
						Text("Hello World", {
							.color = 0xde1818ff,
							.font = fontRegular,
							// .fontSize = 128_px,
							// .fontSize = 24_px,
							.fontSize = 18, // 18pt == 24px
						});
					}

					sequence<char8> text;

					/* TODO(gui/text-smoothness)
					- 16px = (A = 0.15-0.23, B = 0.70-0.64)
					- 18px = (A = 0.12-0.20, B = 0.70-0.60)
					- 22px = (A = 0.35, B = 0.58)
					- 18pt (24px) = (A = 0.4, B = 1.0)
					- 40px = (A = 0.39, B = 0.58)
					- 96px = (A = 0.46, B = 0.53)
					*/

					text = Logger::format("A = []", hintingY);

					// const Color textColor = 0xa5a5a5ff;
					const Color textColor = 0x000000ff;

					Text(text, {
						.color = textColor,
						.font = fontRegular,
						.fontSize = 16_px,
					});

					RangeControl(300_px, draggingA, offsetA, previousOffsetA, hintingY);

					text = Logger::format("B = []", hintingX);

					Text(text, {
						.color = textColor,
						.font = fontRegular,
						.fontSize = 16_px,
					});

					RangeControl(300_px, draggingB, offsetB, previousOffsetB, hintingX);

					Text("(12px) The quick brown fox jumps over the lazy dog.", {
						.color = textColor,
						.font = fontRegular,
						.fontSize = 12_px,
					});
					Text("(18px) The quick brown fox jumps over the lazy dog.", {
						.color = textColor,
						.font = fontBold,
						.fontSize = 18_px,
					});
					Text("(30px) The quick brown fox jumps over the lazy dog.", {
						.color = textColor,
						.font = fontRegularItalic,
						.fontSize = 30_px,
					});
					Text("(128px) The quick brown fox jumps over the lazy dog.", {
						.color = textColor,
						.font = fontBoldItalic,
						.fontSize = 128_px,
					});
				}
			}

			void onMouseUpClient(MouseUpEvent& event) override {
				draggingA = false;
				draggingB = false;
			}

		private:
			void RangeControl(float32 width, bool& dragging, float32& offset, float32& previousOffset, float32& value) {
				Container({
					.size = {width, 23_px},
					.padding = {7_px, 7_px, 7_px, 7_px},
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
			FontID fontBold;
			FontID fontBoldItalic;
			FontID fontRegular;
			FontID fontRegularItalic;

			bool draggingA = false;
			float32 offsetA = 7_px;
			float32 previousOffsetA = 7_px;

			bool draggingB = false;
			float32 offsetB = 7_px;
			float32 previousOffsetB = 7_px;
	};
}