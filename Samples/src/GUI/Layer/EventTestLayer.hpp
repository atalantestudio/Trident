#pragma once

namespace ProjectA {
	class SampleLayer : public GUILayer {
		public:
			void registerAssets() override {
				font = loadFontAsset("test-regular");

				containerStyle.size = {FIT, FIT};
				containerStyle.padding = {8_px, 8_px};
				containerStyle.layoutDirection = VERTICAL;
				containerStyle.layoutSpacing = 8_px;

				buttonEventListeners.onMouseDown = BIND(onMouseDown);
				buttonEventListeners.onMouseUp = BIND(onMouseUp);
				buttonEventListeners.onMouseHover = BIND(onMouseHover);
				buttonEventListeners.onMouseLeave = BIND(onMouseLeave);
				buttonEventListeners.onMouseClick = BIND(onMouseClick);

				buttonStyle.size = {FIT, FIT};
				buttonStyle.padding = {10_px, 8_px};
				buttonStyle.color = 0x757575ff;
				buttonStyle.layoutSpacing = 8_px;
				buttonStyle.outlineColor = 0xd9d9d9ff;
				buttonStyle.eventListeners = &buttonEventListeners;

				textStyle.color = 0x000000ff;
				textStyle.font = font;

				if constexpr (true) {
					// Small
					buttonStyle.outlineSize = 1_px;
					buttonStyle.borderRadius = 5.0f;
					textStyle.fontSize = 16.0f;
				}
				else {
					// Large
					buttonStyle.outlineSize = 5_px;
					buttonStyle.borderRadius = 16.0f;
					textStyle.fontSize = 64.0f;
				}
			}

			void GUI() override {
				GUIComponent(containerStyle) {
					GUIComponent(buttonStyle) {
						GUIText("Button", textStyle);
					}

					if (hovered) {
						GUIText("Hovered", textStyle);
					}

					if (mouseDown) {
						GUIText("Mouse Down", textStyle);
					}
				}
			}

			void onMouseDown(MouseDownEvent& event) {
				mouseDown = true;
			}

			void onMouseUp(MouseUpEvent& event) {
				mouseDown = false;
			}

			void onMouseHover(MouseMoveEvent& event) {
				hovered = true;
			}

			void onMouseLeave(MouseMoveEvent& event) {
				hovered = false;
			}

			void onMouseClick(MouseUpEvent& event) {
				ENGINE_DEBUG("Received MouseUpEvent.");
			}

		private:
			FontID font;
			ContainerStyle containerStyle;
			ContainerStyle buttonStyle;
			TextStyle textStyle;
			EventListeners buttonEventListeners;

			bool mouseDown = false;
			bool hovered = false;
	};
}