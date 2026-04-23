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
					.size = {static_cast<float32>(size[0]), static_cast<float32>(size[1])},
					.anchorPoint = AnchorPoint::HCENTER | AnchorPoint::TOP,
					.color = 0x000000ff,
				}) {
					OptionListContainer();
				}
			}

		private:
			void OptionListContainer() {
				GUIComponent({
					.size = {540_px, EXPAND},
					.padding = {8_px, 8_px},
					.layoutDirection = LayoutDirection::VERTICAL,
					.layoutSpacing = 8_px,
					.color = 0x0a0a0aff,
				}) {
					OptionListTitle("Graphics Options");

					GraphicsOptionList();

					Text("Loaded asset:", {
						.color = 0x828282ff,
						.font = fontRegular,
						.fontSize = 18_px,
					});

					Text(getApplication()->getPropertyFile()->getProperty<view<char8>>("AssetName"), {
						.color = 0x828282ff,
						.font = fontRegular,
						.fontSize = 18_px,
					});
				}
			}

			void OptionListTitle(view<char8> title) {
				GUIComponent({
					.size = {EXPAND, FIT},
					.padding = {8_px, 0_px},
				}) {
					Text(title, {
						.color = 0xa5a5a5ff,
						.font = fontBold,
						.fontSize = 28_px,
					});
				}
			}

			void GraphicsOptionList() {
				GUIComponent({
					.size = {EXPAND, FIT},
					.layoutDirection = LayoutDirection::VERTICAL,
					.layoutSpacing = -1_px,
					.color = 0xff9800ff,
				}) {
					VSyncOptionButton();
					FullScreenOptionButton();
				}
			}

			void VSyncOptionButton() {
				GUIComponent({
					.size = {EXPAND, FIT},
					.padding = {1_px, 1_px},
					.color = 0x3a3a3aff,
					.pointerType = PointerType::INTERACTIVE,
				}) {
					if (IsClicked()) {
						VSync = !VSync;
					}

					GUIComponent({
						.size = {EXPAND, FIT},
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

			void FullScreenOptionButton() {
				GUIComponent({
					.size = {EXPAND, FIT},
					.padding = {1_px, 1_px},
					.color = 0x3a3a3aff,
					.pointerType = PointerType::INTERACTIVE,
				}) {
					if (IsClicked()) {
						FullScreen = !FullScreen;
					}

					GUIComponent({
						.size = {EXPAND, FIT},
						.padding = {10_px, 8_px},
						.layoutDirection = LayoutDirection::VERTICAL,
						.color = 0x0a0a0aff,
						.pointerType = PointerType::INTERACTIVE,
					}) {
						Text("Full-screen", {
							.color = 0xa5a5a5ff,
							.font = fontBold,
							.fontSize = 22_px,
						});

						Text("Run in either a full-screen window or a bordered window.", {
							.color = 0x828282ff,
							.font = fontRegular,
							.fontSize = 18_px,
						});

						if (FullScreen) {
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
			bool FullScreen = false;
	};
}