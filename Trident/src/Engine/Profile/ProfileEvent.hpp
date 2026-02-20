// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct ProfileEvent {
		sequence<char8> name;
		std::thread::id threadId;
		sint64 start;
		sint64 end;
	};
}