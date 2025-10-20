#pragma once

namespace ProjectA {
	typedef char char8;
	typedef char16_t char16;
	typedef char32_t char32;

	typedef std::int8_t sint8;
	typedef std::int16_t sint16;
	typedef std::int32_t sint32;
	typedef std::int64_t sint64;

	typedef std::uint8_t uint8, flag;
	typedef std::uint16_t uint16;
	typedef std::uint32_t uint32;
	typedef std::uint64_t uint64;

	typedef std::float_t float32;
	typedef std::double_t float64;

	template<typename T>
	using ref = std::shared_ptr<T>;
}