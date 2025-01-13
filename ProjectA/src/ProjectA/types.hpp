#pragma once

namespace ProjectA {
	// bool
	// char

	typedef std::byte byte;

	typedef std::int8_t int8;
	typedef std::int16_t int16;
	typedef std::int32_t int32;
	typedef std::int64_t int64;

	typedef std::uint8_t uint8;
	typedef std::uint16_t uint16;
	typedef std::uint32_t uint32;
	typedef std::uint64_t uint64;

	typedef std::float_t float32;
	typedef std::double_t float64;

	// void

	// TODO: vecC

	// TODO: matRxC

	// TODO: quat

	template<typename T>
	using ref = std::shared_ptr<T>;

	// TODO: Deprecate?
	template<typename T>
	using Scope = std::unique_ptr<T>;
}