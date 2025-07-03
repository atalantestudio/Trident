#pragma once

#if OPERATING_SYSTEM == OPERATING_SYSTEM_WINDOWS
	#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>

#ifdef BUILD_MODE_DEBUG
	#include <vulkan/vk_enum_string_helper.h>
#endif