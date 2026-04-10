project "Trident"
	location "."
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	vectorextensions "AVX2"

	targetdir ("../bin/" .. OUTPUT_DIR .. "/%{prj.name}")
	objdir ("../obj/" .. OUTPUT_DIR .. "/%{prj.name}")

	pchheader "pch.hpp"
	pchsource "src/pch.cpp"

	defines {
		"_CRT_SECURE_NO_WARNINGS",
	}

	files {
		"src/**.cpp",
		"vendor/API/GL/glcorearb.h",
		"vendor/API/GL/glext.h",
		"vendor/API/GL/wglext.h",
		"vendor/API/KHR/khrplatform.h",
		"vendor/Base/Base/**.hpp",
		"vendor/lyah/lyah/**.hpp",
		"vendor/scroll/scroll/**.hpp",
		"vendor/tsl/tsl/**.cpp",
		"vendor/tsl/tsl/**.hpp",
	}

	removefiles {
		"src/Engine/Adapter/Graphics/DX12/**.cpp",
		"src/Engine/Adapter/Window/Win32/**.cpp",
		"vendor/tsl/tsl/tslc.cpp",
	}

	includedirs {
		-- TODO(ci/vk/sdk-inclusion): Only include in debug mode, when the validation layers are enabled.
		VULKAN_SDK .. "/Include",

		"src",
		"vendor/API",
		"vendor/Base",
		"vendor/lyah",
		"vendor/scroll",
		"vendor/tsl",
	}

	libdirs {
		VULKAN_SDK .. "/Lib",
	}

	filter "system:windows"
		systemversion "latest"

		files {
			"src/Engine/Adapter/Graphics/DX12/**.cpp",
			"src/Engine/Adapter/Window/Win32/**.cpp",
		}

	filter "configurations:Debug"
		defines {
			"BUILD_MODE_DEBUG",
		}

		runtime "Debug"
		symbols "on"
		optimize "off"

		excludes "src/Engine/Profile/**"

	filter "configurations:Profile"
		defines {
			"BUILD_MODE_PROFILE",
		}

		runtime "Release"
		optimize "on"

	filter "configurations:Release"
		defines {
			"BUILD_MODE_RELEASE",
		}

		runtime "Release"
		optimize "on"

		excludes "src/Engine/Profile/**"