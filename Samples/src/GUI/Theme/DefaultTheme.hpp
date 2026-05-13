// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUIDefaultTheme : public GUILayer {
		public:
			void registerAssets() override {
				fontBold = loadFontAsset("test-bold");
				fontRegular = loadFontAsset("test-regular");

				clearColor = primaryColor;
			}

		protected:
			void updateThemeColors(bool darkMode) {
				// TODO: Blue (0x5cafe7ff)

				if (darkMode) {
					primaryColor = 0x0a0a0aff;
					secondaryColor = 0x3a3a3aff;
					backgroundHoverColor = 0x1a1a1aff;
					borderFocusColor = 0xc2c2c2ff;
					textColor = 0xa5a5a5ff;
					textHoverColor = 0xc2c2c2ff;

					errorNoteColor = 0x2e211fff;
					errorBorderColor = 0xffa198ff;
					infoNoteColor = 0x13232eff;
					infoBorderColor = 0x40b4ffff;
					warningNoteColor = 0x292111ff;
					warningBorderColor = 0xd4a238ff;
				}
				else {
					primaryColor = 0xf0f0f0ff;
					secondaryColor = 0x989898ff;
					backgroundHoverColor = 0xd2d2d2ff;
					borderFocusColor = 0x3a3a3aff;
					textColor = 0x0a0a0aff;
					textHoverColor = 0x535353ff;

					errorNoteColor = 0xe0d9d8ff;
					errorBorderColor = 0x855651ff;
					infoNoteColor = 0xd2dae0ff;
					infoBorderColor = 0x255f85ff;
					warningNoteColor = 0xddd9d1ff;
					warningBorderColor = 0x6f5621ff;
				}

				clearColor = primaryColor;
			}

			void InfoNote(view<char8> note) {
				Note(note, infoNoteColor, infoBorderColor);
			}

			void ErrorNote(view<char8> note) {
				Note(note, errorNoteColor, errorBorderColor);
			}

			void WarningNote(view<char8> note) {
				Note(note, warningNoteColor, warningBorderColor);
			}

			bool Button(view<char8> label) {
				bool clicked = false;
				Color borderColor = secondaryColor;

				if (IsNextMouseDown()) {
					borderColor = borderFocusColor;
				}

				Container({
					.padding = {1_px, 1_px, 1_px, 1_px},
					.color = borderColor,
					.pointerType = PointerType::INTERACTIVE,
				}) {
					Color buttonColor = primaryColor;
					Color labelColor = textColor;

					if (IsHovered()) {
						buttonColor = backgroundHoverColor;
						labelColor = textHoverColor;
					}

					clicked = IsClicked();

					Container({
						.padding = {5_px, 5_px, 0_px, 0_px},
						.color = buttonColor,
					}) {
						Text(label, {
							.color = labelColor,
							.font = fontRegular,
							.fontSize = 16_px,
						});
					}
				}

				return clicked;
			}

			void RangeControl(bool& dragging, float32& previousOffset, float32& value) {
				Container({
					.size = {EXPAND, 23_px},
					.padding = {0_px, 0_px, 0_px, 0_px},
					.color = primaryColor,
				}) {
					RangeControlTrack();
					RangeControlThumb(dragging, previousOffset, value);
				}
			}

		private:
			void Note(view<char8> note, Color noteColor, Color borderColor) {
				Container({
					.size = {EXPAND, FIT},
					.padding = {4_px, 1_px, 1_px, 1_px},
					.color = borderColor,
				}) {
					Container({
						.size = {EXPAND, FIT},
						.padding = {16_px, 16_px, 16_px, 16_px},
						.color = noteColor,
					}) {
						Text(note, {
							.color = borderColor,
							.font = fontRegular,
							.fontSize = 17.6_px,
						});
					}
				}
			}

			void RangeControlTrack() {
				Floating({
					.offset = {0_px, 11_px},
					.size = {EXPAND, 1_px},
					.color = secondaryColor,
				});
			}

			void RangeControlThumb(bool& dragging, float32& previousOffset, float32& value) {
				if (CurrentComponent() == nullptr) {
					return;
				}

				const float32 width = CurrentComponent()->size.width.value;

				if (dragging) {
					const float32 currentOffset = getMousePosition()[0];

					value += (currentOffset - previousOffset) / width;
					previousOffset = currentOffset;

					value = clamp(value, 0.0f, 1.0f);
				}

				Floating({
					.offset = {value * width - 7_px, 0.0f},
					// .size = {FIT, FIT},
					.size = {14_px, 23_px},
					// .padding = {1_px, 1_px, 1_px, 1_px},
					.color = textHoverColor,
					.pointerType = PointerType::INTERACTIVE,
				}) {
					if (IsMouseDown()) {
						dragging = true;
						previousOffset = getMousePosition()[0];
					}

					if (IsMouseUp()) {
						dragging = false;
					}

					// TODO
					/* Container({
						.size = {12_px, 18_px},
						.color = 0x5cafe7ff,
					}); */
				}
			}

		protected:
			Color primaryColor = 0x00000000;
			Color secondaryColor = 0x00000000;

			Color backgroundHoverColor = 0x00000000;
			Color borderFocusColor = 0x00000000;

			Color textColor = 0x00000000;
			Color textHoverColor = 0x00000000;

			Color errorNoteColor = 0xe0d9d8ff;
			Color errorBorderColor = 0x855651ff;

			Color infoNoteColor = 0xd2dae0ff;
			Color infoBorderColor = 0x255f85ff;

			Color warningNoteColor = 0xddd9d1ff;
			Color warningBorderColor = 0x6f5621ff;

			FontID fontBold;
			FontID fontRegular;
	};
}