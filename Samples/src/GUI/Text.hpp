// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUITextLayer : public GUILayer {
		public:
			void registerAssets() override {
				font = loadFontAsset("test-regular");
				fontItalic = loadFontAsset("test-regular");
			}

			void render() override {
				const Extent2D extent = getApplication()->getWindow()->getExtent();

				Container({
					.size = {static_cast<float32>(extent.width), static_cast<float32>(extent.height)},
					.layoutDirection = LayoutDirection::VERTICAL,
					.color = 0xffffffff,
				}) {
					float32 fontSize = 30_px;

					while (fontSize >= 10_px) {
						TextSample("The quick brown fox jumps over the lazy dog.", fontSize);

						fontSize -= 1_px;
					}
				}
			}

		protected:
			void TextSample(view<char8> text, float32 fontSize) {
				const sequence<char8> label = Logger::format("[]px", static_cast<sint32>(fontSize));

				Container({
					.size = {FIT, FIT},
					.anchorPoint = AnchorPoint::VCENTER,
					.layoutDirection = LayoutDirection::HORIZONTAL,
				}) {
					Container({
						.size = {20_px, FIT},
						.padding = {8_px, 8_px, 0_px, 0_px},
					}) {
						Text(label, {
							.color = 0xa5a5a5ff,
							.font = fontItalic,
							.fontSize = 10_px,
						});
					}

					Container({
						.size = {FIT, FIT},
					}) {
						Text(text, {
							.color = 0x000000ff,
							.font = font,
							.fontSize = fontSize,
						});
					}
				}
			}

		private:
			FontID font;
			FontID fontItalic;
	};
}