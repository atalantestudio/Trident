// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/GUI/Component/Component.hpp"
#include "Engine/API/GUI/GUILayer.hpp"

namespace Trident {
	class Button : public GUIComponent {
		public:
			explicit Button() {
				eventListeners.onMouseDown = BIND(onMouseDown);
				eventListeners.onMouseUp = BIND(onMouseUp);
				eventListeners.onMouseHover = BIND(onMouseHover);
				eventListeners.onMouseLeave = BIND(onMouseLeave);

				style.size = {GUILayer::FIT, GUILayer::FIT};
				style.padding = {1_px, 1_px};
				style.color = 0x3a3a3aff;
				style.pointerType = PointerType::INTERACTIVE;
				style.eventListeners = &eventListeners;

				innerStyle.size = {GUILayer::FIT, GUILayer::FIT};
				innerStyle.padding = {10_px, 8_px};
				innerStyle.layoutDirection = VERTICAL;	
				innerStyle.color = 0x0a0a0aff;
			}

			void render(std::function<void(const ContainerStyle&)> openComponent, std::function<void()> closeComponent, std::function<void()> renderChildren) const override {
				GUIComponent(style) {
					GUIComponent(innerStyle) {
						renderChildren();
					}
				}
			}

			void onMouseDown(MouseDownEvent& event) {
				style.zIndex = 1;
				style.color = 0xc2c2c2ff;

				innerStyle.zIndex = 1;

				// TODO: Propagate text color to children.
				// textStyle.color = 0xc2c2c2ff;
			}

			void onMouseUp(MouseUpEvent& event) {
				style.zIndex = 0;
				style.color = 0x3a3a3aff;

				innerStyle.zIndex = 0;

				// TODO: Propagate text color to children.
				// textStyle.color = 0xa5a5a5ff;
			}

			void onMouseHover(MouseMoveEvent& event) {
				// innerStyle.zIndex = 1;
				innerStyle.color = 0x1a1a1aff;
			}

			void onMouseLeave(MouseMoveEvent& event) {
				innerStyle.color = 0x0a0a0aff;
			}

		public:
			ContainerStyle style;
			ContainerStyle innerStyle;
			EventListeners eventListeners;
	};
}