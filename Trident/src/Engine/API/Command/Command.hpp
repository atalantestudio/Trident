// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct Command {
		view<char8> name;
		sequence<view<char8>> arguments;
	};

	Command parseCommand(view<char8> text);
}