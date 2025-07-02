#pragma once

#define BIND(function) [this](auto&&... arguments) { \
	return this->function(std::forward<decltype(arguments)>(arguments)...); \
}