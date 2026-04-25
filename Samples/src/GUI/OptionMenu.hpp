// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUIOptionMenuLayer : public GUILayer {
		private:
			enum class OptionMenuTab : uint8 {
				CONTROLS,
				ACCESSIBILITY,

				count,
			};

			static constexpr const view<char8> OPTION_MENU_TAB_STRINGS[] = {
				"Controls",
				"Accessibility",
			};

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
					.layoutDirection = LayoutDirection::VERTICAL,
					.color = 0x000000ff,
					.debugName = "WindowContainer",
				}) {
					GUIComponent({
						.size = {EXPAND, 48_px},
						.anchorPoint = AnchorPoint::HCENTER | AnchorPoint::TOP,
						.color = 0x0a0a0aff,
						.debugName = "OptionMenuTabListContainer",
					}) {
						OptionMenuTabList();
					}

					GUIComponent({
						.size = {548_px, EXPAND},
						.padding = {8_px, 8_px},
						.layoutDirection = LayoutDirection::VERTICAL,
						.color = 0x0a0a0aff,
						.debugName = "OptionMenuContainer",
					}) {
						switch (optionMenuTab) {
							case OptionMenuTab::CONTROLS: {
								GraphicsOptionList();

								break;
							}
							case OptionMenuTab::ACCESSIBILITY: {
								AccessibilityOptionList();

								break;
							}
						}
					}
				}
			}

		private:
			void OptionMenuTabList() {
				GUIComponent({
					.size = {548_px, EXPAND},
					.anchorPoint = AnchorPoint::HCENTER | AnchorPoint::TOP,
				}) {
					uint8 optionMenuTabIndex = 0;

					while (optionMenuTabIndex < static_cast<uint8>(OptionMenuTab::count)) {
						OptionMenuTabButton(static_cast<OptionMenuTab>(optionMenuTabIndex));

						optionMenuTabIndex += 1;
					}
				}
			}

			void OptionMenuTabButton(OptionMenuTab tab) {
				Color backgroundColor = 0x0a0a0aff;

				if (IsNextHovered()) {
					backgroundColor = 0x1a1a1aff;
				}

				GUIComponent({
					.size = {FIT, EXPAND},
					.padding = {10_px, 0_px},
					.anchorPoint = AnchorPoint::HCENTER | AnchorPoint::VCENTER,
					.color = backgroundColor,
					.pointerType = PointerType::INTERACTIVE,
				}) {
					if (IsClicked()) {
						optionMenuTab = tab;
					}

					Text(OPTION_MENU_TAB_STRINGS[static_cast<uint8>(tab)], {
						.color = 0xa5a5a5ff,
						.font = fontBold,
						.fontSize = 22_px,
					});
				}
			}

			void OptionListContainer() {
				GUIComponent({
					.size = {540_px, EXPAND},
					.padding = {8_px, 8_px},
					.layoutDirection = LayoutDirection::VERTICAL,
					.layoutSpacing = 8_px,
					.color = 0x0a0a0aff,
				}) {
					GraphicsOptionList();
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
				}
			}

			void AccessibilityOptionList() {
				GUIComponent({
					.size = {EXPAND, FIT},
					.layoutDirection = LayoutDirection::VERTICAL,
					.layoutSpacing = -1_px,
					.color = 0xff9800ff,
				}) {
					FullScreenOptionButton();
				}
			}

			void VSyncOptionButton() {
				uint32 borderZIndex = 0;
				Color borderColor = 0x3a3a3aff;

				if (IsNextMouseDown()) {
					borderZIndex = 1;
					borderColor = 0xc2c2c2ff;
				}

				GUIComponent({
					.size = {EXPAND, FIT},
					.zIndex = borderZIndex,
					.padding = {1_px, 1_px},
					.color = borderColor,
					.pointerType = PointerType::INTERACTIVE,
				}) {
					Color backgroundColor = 0x0a0a0aff;

					if (IsNextHovered()) {
						backgroundColor = 0x1a1a1aff;
					}

					if (IsClicked()) {
						VSync = !VSync;
					}

					GUIComponent({
						.size = {EXPAND, FIT},
						.padding = {10_px, 8_px},
						.layoutDirection = LayoutDirection::VERTICAL,
						.color = backgroundColor,
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
				uint32 borderZIndex = 0;
				Color borderColor = 0x3a3a3aff;

				if (IsNextMouseDown()) {
					borderZIndex = 1;
					borderColor = 0xc2c2c2ff;
				}

				GUIComponent({
					.size = {EXPAND, FIT},
					.zIndex = borderZIndex,
					.padding = {1_px, 1_px},
					.color = borderColor,
					.pointerType = PointerType::INTERACTIVE,
				}) {
					Color backgroundColor = 0x0a0a0aff;

					if (IsNextHovered()) {
						backgroundColor = 0x1a1a1aff;
					}

					if (IsClicked()) {
						FullScreen = !FullScreen;
					}

					GUIComponent({
						.size = {EXPAND, FIT},
						.padding = {10_px, 8_px},
						.layoutDirection = LayoutDirection::VERTICAL,
						.color = backgroundColor,
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

			OptionMenuTab optionMenuTab = OptionMenuTab::CONTROLS;

			bool VSync = false;
			bool FullScreen = false;
	};
}