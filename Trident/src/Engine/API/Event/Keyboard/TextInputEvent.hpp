// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Event/Event.hpp"
#include "Engine/Base/KeyCode.hpp"

namespace Trident {
	struct TextInputEvent : public Event {
		explicit TextInputEvent(uint32 characterCode, uint8 byteSize) :
			characterCode(characterCode),
			byteSize(byteSize)
		{}

		EVENT_TYPE(EventType::TEXT_INPUT)
		EVENT_CATEGORY(EventCategory::INPUT | EventCategory::KEYBOARD)

		/// UTF-8 code bytes.
		uint32 characterCode;

		/// Number of occupied bytes in `characterCode`, in the [1, 4] range.
		uint8 byteSize;
	};
}