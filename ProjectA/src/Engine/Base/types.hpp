#pragma once

namespace ProjectA {
	template<typename T>
	using ref = std::shared_ptr<T>;

	template<typename T>
	using scope = std::unique_ptr<T>;
}