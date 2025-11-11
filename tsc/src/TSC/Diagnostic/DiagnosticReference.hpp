#pragma once

#include "../DocumentRange.hpp"

namespace ProjectA {
	struct DiagnosticReference {
		std::string getMessage() const {
			return std::string(&message[0], message.count());
		}

		DocumentRange range;
		str8 message;
	};
}