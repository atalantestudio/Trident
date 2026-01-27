#pragma once

namespace Trident {
	template<typename Event>
	using EventListener = std::function<void(Event& event)>;
}