// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Event/Event.hpp"
#include "Engine/Base/KeyCode.hpp"
#include "Engine/Filesystem/UTF8String.hpp"

namespace Trident {
	struct TextInputEvent : public Event {
		explicit TextInputEvent(uint32 characterCode, uint8 byteSize) :
			characterCode(characterCode, byteSize)
		{}

		EVENT_TYPE(EventType::TEXT_INPUT)
		EVENT_CATEGORY(EventCategory::INPUT | EventCategory::KEYBOARD)

		/// UTF-8 code bytes.
		UTF8String characterCode;
	};
}