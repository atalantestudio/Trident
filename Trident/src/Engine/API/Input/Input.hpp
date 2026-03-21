// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Event/Keyboard/KeyDownEvent.hpp"
#include "Engine/API/Event/Keyboard/KeyUpEvent.hpp"

namespace Trident {
	class Input {
		public:
			bool operator[](KeyCode keyCode) const {
				DEBUG_ASSERT(keyCode < KeyCode::count);

				return keyMap[static_cast<uint32>(keyCode)];
			}

			void update(KeyDownEvent& event) {
				keyMap[static_cast<uint32>(event.getKeyCode())] = true;
			}

			void update(KeyUpEvent& event) {
				keyMap[static_cast<uint32>(event.getKeyCode())] = false;
			}

		private:
			bool keyMap[static_cast<uint32>(KeyCode::count)]{};
	};
}