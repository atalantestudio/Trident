#pragma once

// TODO: Profile-only PCH header?
#include <string>
#include <thread>

namespace ProjectA {
	struct ProfileEvent {
		const std::string name;
		std::thread::id threadId;
		long long start;
		long long end;
	};
}