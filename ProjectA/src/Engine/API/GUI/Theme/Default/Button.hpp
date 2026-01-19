// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/Component/Component.hpp"

namespace ProjectA {
	class Button : public GUIComponent {
		public:
			explicit Button() {
				containerStyle.size = {GUILayer::FIT, GUILayer::FIT};
				containerStyle.padding = {1_px, 1_px};
				containerStyle.color = 0x3a3a3aff;

				eventListeners.onMouseDown = BIND(onMouseDown);
				eventListeners.onMouseUp = BIND(onMouseUp);
				eventListeners.onMouseHover = BIND(onMouseHover);
				eventListeners.onMouseLeave = BIND(onMouseLeave);

				style.size = {GUILayer::FIT, GUILayer::FIT};
				style.padding = {10_px, 8_px};
				style.color = 0x0a0a0aff;
				style.eventListeners = &eventListeners;
			}

			void render(GUILayer& layer, std::function<void(const ContainerStyle&)> openComponent, std::function<void()> closeComponent, std::function<void(GUILayer&)> renderChildren) const override {
				GUIComponent(containerStyle) {
					GUIComponent(style) {
						renderChildren(layer);
					}
				}
			}

			void onMouseDown(MouseDownEvent& event) {
				mouseDown = true;
				containerStyle.color = 0xc2c2c2ff;

				// TODO: Propagate text color to children.
				// textStyle.color = 0xc2c2c2ff;
			}

			void onMouseUp(MouseUpEvent& event) {
				mouseDown = false;
				containerStyle.color = 0x3a3a3aff;

				// TODO: Propagate text color to children.
				// textStyle.color = 0xa5a5a5ff;
			}

			void onMouseHover(MouseMoveEvent& event) {
				hovered = true;
				style.color = 0x1a1a1aff;
			}

			void onMouseLeave(MouseMoveEvent& event) {
				hovered = false;
				style.color = 0x0a0a0aff;
			}

		private:
			ContainerStyle containerStyle;
			ContainerStyle style;
			EventListeners eventListeners;
			bool mouseDown = false;
			bool hovered = false;
	};
}