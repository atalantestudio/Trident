#pragma once

namespace ProjectA {
	class SampleLayer : public GUILayer {
		public:
			void registerAssets() override {
				containerStyle.size = {EXPAND, EXPAND};
				containerStyle.padding = {16_px, 16_px};
				containerStyle.layoutSpacing = 8_px;

				buttonEventListeners.onMouseDown = BIND(onMouseDown);
				buttonEventListeners.onMouseUp = BIND(onMouseUp);
				buttonEventListeners.onMouseHover = BIND(onMouseHover);
				buttonEventListeners.onMouseLeave = BIND(onMouseLeave);
				buttonEventListeners.onMouseClick = BIND(onMouseClick);

				buttonStyle.size = {FIT, FIT};
				buttonStyle.padding = {10_px, 8_px};
				buttonStyle.layoutSpacing = 8_px;
				buttonStyle.eventListeners = &buttonEventListeners;

				textStyle.color = 0xffffffff;
				textStyle.font = loadFontAsset("DefaultFont");
				textStyle.fontSize = 25.0f;
			}

			void GUI() override {
				GUIComponent(containerStyle) {
					if (hovered) {
						if (mouseDown) {
							buttonStyle.color = 0x303030ff;
						}
						else {
							buttonStyle.color = 0x757575ff;
						}
					}
					else {
						buttonStyle.color = 0x505050ff;
					}

					GUIComponent(buttonStyle) {
						GUIText("Button", textStyle);
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