#include "pch.hpp"

namespace ProjectA {
	ConsoleLogger engineLogger(std::cout, LogLevel::ALL, "ENGINE");

	#ifdef ENABLE_OPENGL_ADAPTER
		ConsoleLogger openGlLogger(std::cout, LogLevel::ALL, "OPENGL");
	#endif

	#ifdef ENABLE_VULKAN_ADAPTER
		ConsoleLogger vulkanLogger(std::cout, LogLevel::ALL, "VULKAN");
	#endif
}