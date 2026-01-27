// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	template<typename Event>
	using EventListener = std::function<void(Event& event)>;
}