// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/API/Command/Command.hpp"
#include "Engine/Filesystem/Stream/UTF8Stream.hpp"

namespace Trident {
	Command parseCommand(view<char8> text) {
		Command command{};

		UTF8Stream stream(text);

		while (stream.getRemainingByteSize() > 0 && std::isspace(stream.peek())) {
			stream.jump();
		}

		const uint64 nameStartOffset = stream.getByteOffset();

		while (stream.getRemainingByteSize() > 0 && !std::isspace(stream.peek())) {
			stream.jump();
		}

		const uint64 nameEndOffset = stream.getByteOffset();

		command.name = view<char8>(&text[nameStartOffset], nameEndOffset - nameStartOffset);

		std::vector<view<char8>> arguments;

		while (stream.getRemainingByteSize() > 0) {
			while (stream.getRemainingByteSize() > 0 && std::isspace(stream.peek())) {
				stream.jump();
			}

			const uint64 argumentStartOffset = stream.getByteOffset();

			while (stream.getRemainingByteSize() > 0 && !std::isspace(stream.peek())) {
				stream.jump();
			}

			const uint64 argumentEndOffset = stream.getByteOffset();

			arguments.emplace_back(view<char8>(&text[argumentStartOffset], argumentEndOffset - argumentStartOffset));
		}

		command.arguments = sequence<view<char8>>(arguments.size());

		std::copy(arguments.begin(), arguments.end(), &command.arguments[0]);

		return command;
	}
}