outputdir = "%{cfg.system}_%{cfg.architecture}_%{cfg.buildcfg}"
vulkansdk = os.getenv("VULKAN_SDK")

workspace "Trident"
	architecture "x86_64"
	startproject "Samples"

	buildoptions {
		"-Wall",
		"-Wextra",
	}

	configurations {
		"Debug",
		"Profile",
		"Release",
	}

	multiprocessorcompile "on"

	filter "system:windows"
		links {
			"Gdi32",
			"User32",
		}

	filter "toolset:clang or toolset:gcc"
		buildoptions {
			-- "-gcodeview",
			"-Wno-missing-designated-field-initializers",
			"-Wno-unused-parameter",
		}

include "Trident"
include "AssetBuilder"
include "Samples"

project "tslc"
	location "Trident/vendor/tsl"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	vectorextensions ("AVX2", "AVX", "SSE4.1", "SSE3", "SSE2", "SSE")

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")

	dependson {
		"Trident",
	}

	files {
		"Trident/vendor/tsl/**.cpp",
		"Trident/vendor/tsl/**.hpp",
	}

	removefiles {
		"Trident/vendor/tsl/tsl/LSP/lsp.cpp",
	}

	includedirs {
		"Trident/src",
		"Trident/vendor/API", -- TODO: Remove.
		"Trident/vendor/Base",
		"Trident/vendor/lyah", -- TODO: Remove.
		"Trident/vendor/scroll",
		"Trident/vendor/tsl",
		vulkansdk .. "/Include", -- TODO: Remove.
	}

	links {
		"Trident",
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines {
			"BUILD_MODE_DEBUG",
		}

		runtime "Debug"
		symbols "on"

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

project "tsllsp"
	location "Trident/vendor/tsl"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	vectorextensions ("AVX2", "AVX", "SSE4.1", "SSE3", "SSE2", "SSE")

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")

	dependson {
		"Trident",
	}

	files {
		"Trident/vendor/tsl/**.cpp",
		"Trident/vendor/tsl/**.hpp",
	}

	removefiles {
		"Trident/vendor/tsl/tsl/tslc.cpp",
	}

	includedirs {
		"Trident/src",
		"Trident/vendor/API", -- TODO: Remove.
		"Trident/vendor/Base",
		"Trident/vendor/lyah", -- TODO: Remove.
		"Trident/vendor/scroll",
		"Trident/vendor/tsl",
		vulkansdk .. "/Include", -- TODO: Remove.
	}

	links {
		"Trident",
	}

	filter "system:windows"
		systemversion "latest"

	filter "_ACTION:vs2019 or _ACTION:vs2022 or _ACTION:vs2026"
		postbuildcommands {
			-- TODO(debug-only): Test LSP server executable inside IDE.
			"{COPYFILE} %[bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.exe] C:/Users/Mattéo/.vscode/extensions/tsl/src/lsp.exe", -- TODO: (debug-only)
		}

	filter "configurations:Debug"
		defines {
			"BUILD_MODE_DEBUG",
		}

		runtime "Debug"
		symbols "on"

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