// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/Theme/Default/Button.hpp"

namespace Trident {
	class GUIOptionMenuLayer : public GUILayer {
		public:
			void registerAssets() override {
				fontBold = loadFontAsset("test-bold");
				fontRegular = loadFontAsset("test-regular");
			}

			void GUI() override {
				const sint32* size = getApplication()->getWindow()->getSize();

				GUIComponent({
					.size = {static_cast<float32>(size[0] - 24_px), static_cast<float32>(size[1] - 16_px)},
					.padding = {8_px, 8_px},
					.layoutDirection = VERTICAL,
					.layoutSpacing = 8_px,
					.color = 0x000000ff,
				}) {
					VSyncOptionButton();
				}
			}

		private:
			void VSyncOptionButton() {
				GUIComponent({
					.size = {FIT, FIT},
					.padding = {1_px, 1_px},
					.color = 0x3a3a3aff,
					.pointerType = PointerType::INTERACTIVE,
				}) {
					if (IsClicked()) {
						VSync = !VSync;
					}

					GUIComponent({
						.size = {200_px, FIT},
						.padding = {10_px, 8_px},
						.layoutDirection = LayoutDirection::VERTICAL,
						.color = 0x0a0a0aff,
						.pointerType = PointerType::INTERACTIVE,
					}) {
						Text("V-Sync", {
							.color = 0xa5a5a5ff,
							.font = fontBold,
							.fontSize = 22_px,
						});

						Text("Vertical synchronization.", {
							.color = 0x828282ff,
							.font = fontRegular,
							.fontSize = 18_px,
						});

						if (VSync) {
							Text("Enabled", {
								.color = 0x80ef80ff,
								.font = fontRegular,
								.fontSize = 18_px,
							});
						}
						else {
							Text("Disabled", {
								.color = 0xff474cff,
								.font = fontRegular,
								.fontSize = 18_px,
							});
						}
					}
				}
			}

		private:
			FontID fontBold;
			FontID fontRegular;

			bool VSync = false;
	};
}