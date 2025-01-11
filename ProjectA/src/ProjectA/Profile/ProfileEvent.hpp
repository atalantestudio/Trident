#pragma once

#include "pch.hpp"

namespace ProjectA {
	struct ProfileEvent {
		const std::string name;
		std::thread::id threadId;
		long long start;
		long long end;
	};
}