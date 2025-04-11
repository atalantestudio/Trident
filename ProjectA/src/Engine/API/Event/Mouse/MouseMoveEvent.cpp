#include "pch.hpp"

#include "Engine/API/Event/Mouse/MouseMoveEvent.hpp"

namespace ProjectA {
	MouseMoveEvent::MouseMoveEvent(sint32 x, sint32 y) :
		x(x),
		y(y)
	{}

	std::string MouseMoveEvent::toString() const {
		std::stringstream stream;

		stream << getName() << ": " << x << ", " << y;

		return stream.str();
	}
}