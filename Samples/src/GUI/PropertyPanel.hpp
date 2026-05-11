// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUIPropertyPanelLayer : public GUILayer {
		private:
			inline static const Color PRIMARY_COLOR = 0xf0f0f0ff;
			inline static const Color SECONDARY_COLOR = 0x989898ff;
			inline static const Color TERTIARY_COLOR = 0xd2d2d2ff;
			inline static const Color BORDER_FOCUS_COLOR = 0x3a3a3aff;
			inline static const Color TEXT_COLOR = 0x0a0a0aff;
			inline static const Color TEXT_HOVER_COLOR = 0x535353ff;

		public:
			void registerAssets() override {
				fontBold = loadFontAsset("test-bold");
				fontRegular = loadFontAsset("test-regular");
			}

			void render() override {
				const Extent2D extent = getApplication()->getWindow()->getExtent();

				Container({
					.size = {static_cast<float32>(extent.width - 16_px), static_cast<float32>(extent.height - 16_px)},
					.padding = {8_px, 8_px},
					.layoutDirection = LayoutDirection::VERTICAL,
					.layoutSpacing = 8_px,
					.color = PRIMARY_COLOR,
				}) {
					Button("Light");
				}
			}

		private:
			void Button(view<char8> label) {
				Color borderColor = SECONDARY_COLOR;

				if (IsNextMouseDown()) {
					borderColor = BORDER_FOCUS_COLOR;
				}

				Container({
					.padding = {1_px, 1_px},
					.color = borderColor,
					.pointerType = PointerType::INTERACTIVE,
				}) {
					Color buttonColor = PRIMARY_COLOR;
					Color labelColor = TEXT_COLOR;

					if (IsHovered()) {
						buttonColor = TERTIARY_COLOR;
						labelColor = TEXT_HOVER_COLOR;
					}

					Container({
						.padding = {5_px, 0_px},
						.color = buttonColor,
					}) {
						Text(label, {
							.color = labelColor,
							.font = fontRegular,
							.fontSize = 16_px,
						});
					}
				}
			}

		private:
			FontID fontBold;
			FontID fontRegular;
	};
}