// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	class GUIMenuBarLayer : public GUILayer {
		private:
			inline static const sequence<sequence<char8>> MENU_ITEMS = {
				"File",
				"Edit",
				"Help",
			};

			inline static const sequence<sequence<sequence<char8>>> CONTEXT_MENU_ITEMS = {
				// File
				{
					"New File",
					"Open File",
					"Save",
					"Save As...",
				},

				// Edit
				{
					"Undo",
					"Redo",
					"Cut",
					"Copy",
					"Paste",
					"Find",
					"Replace",
				},

				// Help
				{
					"About",
					"Documentation",
					"Report Issue",
					"Check For Updates",
				},
			};

		public:
			void registerAssets() override {
				fontBold = loadFontAsset("test-bold");
				fontRegular = loadFontAsset("test-regular");

				rootColor = 0xf0f0f0ff;
			}

			void render() override {
				MenuBar();

				Container({
					.size = {FIT, FIT},
					.padding = {8_px, 24_px},
				}) {
					Text("Home", {
						.color = textColor,
						.font = fontBold,
						.fontSize = 35.2_px,
					});
				}
			}

		private:
			void MenuBar() {
				Container({
					.size = {EXPAND, FIT},
					.layoutDirection = LayoutDirection::HORIZONTAL,
				}) {
					index itemIndex = 0;

					while (itemIndex < MENU_ITEMS.count()) {
						MenuItem(itemIndex);

						itemIndex += 1;
					}
				}
			}

			void MenuItem(index menuIndex) {
				Container({
					.size = {FIT, FIT},
					.layoutDirection = LayoutDirection::VERTICAL,
				}) {
					Color color = 0x00000000;
					Color labelColor = 0x535353ff;

					if (IsNextHovered()) {
						color = 0xd2d2d2ff;
						labelColor = textColor;
					}

					Container({
						.size = {FIT, FIT},
						.padding = {8_px, 4_px},
						.color = color,
						.pointerType = PointerType::INTERACTIVE,
					}) {
						if (IsClicked()) {
							if (openedMenuIndex == menuIndex) {
								openedMenuIndex = max<uint32>();
							}
							else {
								openedMenuIndex = menuIndex;
							}
						}

						FontID labelFont = fontRegular;

						if (openedMenuIndex == menuIndex) {
							labelFont = fontBold;
						}

						Text(MENU_ITEMS[menuIndex], {
							.color = labelColor,
							.font = labelFont,
							.fontSize = 19_px,
						});
					}

					if (openedMenuIndex == menuIndex) {
						ContextMenu(openedMenuIndex);
					}
				}
			}

			void ContextMenu(index menuIndex) {
				const sequence<sequence<char8>>& contextMenuItems = CONTEXT_MENU_ITEMS[menuIndex];

				Floating({
					.size = {FIT, FIT},
					.zIndex = 1,
					.padding = {1_px, 1_px},
					.layoutDirection = LayoutDirection::VERTICAL,
					.layoutSpacing = 1_px,
					.color = 0x989898ff,
				}) {
					index itemIndex = 0;

					while (itemIndex < contextMenuItems.count()) {
						ContextMenuItem(contextMenuItems[itemIndex]);

						itemIndex += 1;
					}
				}
			}

			void ContextMenuItem(view<char8> name) {
				Container({
					.size = {EXPAND, FIT},
					.zIndex = 1,
					.padding = {8_px, 4_px},
					.color = 0xf0f0f0ff,
					.pointerType = PointerType::INTERACTIVE,
				}) {
					Text(name, {
						.color = textColor,
						.font = fontRegular,
						.fontSize = 19_px,
					});
				}
			}

		private:
			FontID fontBold;
			FontID fontRegular;
			Color textColor = 0x0a0a0aff;
			index openedMenuIndex = max<index>();
	};
}