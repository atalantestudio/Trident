project "ProjectB"
	location "."
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	vectorextensions "AVX2"

	targetdir ("../bin/" .. OUTPUT_DIR .. "/%{prj.name}")
	objdir ("../obj/" .. OUTPUT_DIR .. "/%{prj.name}")

	dependson {
		"Trident",
	}

	files {
		"src/**.cpp",
		"src/**.hpp",
	}

	includedirs {
		VULKAN_SDK .. "/Include",
		"../ProjectB/src",
		"../Trident/src",
		"../Trident/vendor/API",
		"../Trident/vendor/Base",
		"../Trident/vendor/lyah",
		"../Trident/vendor/scroll",
		"../Trident/vendor/tsl",
	}

	links {
		"Trident",
	}

	defines {
		"PROJECT_NAME=\"%{prj.name}\"",
	}

	filter "system:windows"
		links {
			"d3d12",
			"dxgi",
			"Gdi32",
			"User32",
		}

	filter "toolset:msc-v142 or toolset:msc-v143 or toolset:msc-v145"
		postbuildcommands {
			-- If a .tpr file exists, use it as the default configuration file.
			"IF EXIST %[%{prj.name}.tpr] {COPYFILE} %[%{prj.name}.tpr] %[../bin/" .. OUTPUT_DIR .. "/%{prj.name}/%{prj.name}.tpr]",
		}

	filter "configurations:Debug"
		defines {
			"BUILD_MODE_DEBUG",
		}

		runtime "Debug"
		symbols "on"
		optimize "off"

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