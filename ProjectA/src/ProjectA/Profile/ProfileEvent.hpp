#pragma once

namespace ProjectA {
	struct ProfileEvent {
		std::string name;
		std::thread::id threadId;
		long long start;
		long long end;
	};
}