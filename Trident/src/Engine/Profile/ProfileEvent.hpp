#pragma once

namespace Trident {
	struct ProfileEvent {
		std::string name;
		std::thread::id threadId;
		sint64 start;
		sint64 end;
	};
}