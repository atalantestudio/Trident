#pragma once

namespace ProjectA {
	template<typename Event>
	using EventListener = std::function<void(Event& event)>;
}