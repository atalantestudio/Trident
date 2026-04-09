OUTPUT_DIR = "%{cfg.system}_%{cfg.architecture}_%{cfg.buildcfg}"
VULKAN_SDK = os.getenv("VULKAN_SDK")

workspace "Trident"
	architecture "x86_64"
	startproject "Samples"
	multiprocessorcompile "on"

	configurations {
		"Debug",
		"Profile",
		"Release",
	}

	filter "system:windows"
		links {
			"d3d12",

			-- TODO(graphics/dx12/shader-compilation): Remove when finishing TSL shader modules (they will embed DXIL output).
			"d3dcompiler",

			"dxgi",

			-- TODO(graphics/dx12/report-live-objects): Don't include when DEBUG_DX12_REPORT_LIVE_OBJECTS is not defined.
			"dxguid",

			"Gdi32",
			"User32",
		}

	filter "toolset:clang or toolset:gcc"
		buildoptions {
			-- TODO(ci/breakpoints): Make breakpoints works with Clang/GCC.
			-- "-gcodeview",

			"-Wall",
			"-Wextra",
			"-Wno-unused-parameter",
		}

		filter "toolset:clang"
			buildoptions {
				"-Wno-missing-designated-field-initializers",
			}

		filter "toolset:gcc"
			buildoptions {
				"-Wno-missing-field-initializers",
				"-Wno-init-list-lifetime",
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

	targetdir ("bin/" .. OUTPUT_DIR .. "/%{prj.name}")
	objdir ("obj/" .. OUTPUT_DIR .. "/%{prj.name}")

	dependson {
		"Trident",
	}

	files {
		"Trident/vendor/tsl/tsl/**.cpp",
		"Trident/vendor/tsl/tsl/**.hpp",
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
		-- TODO(ci/vk/sdk-inclusion): Only include in debug mode, when the validation layers are enabled.
		VULKAN_SDK .. "/Include",
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

	targetdir ("bin/" .. OUTPUT_DIR .. "/%{prj.name}")
	objdir ("obj/" .. OUTPUT_DIR .. "/%{prj.name}")

	dependson {
		"Trident",
	}

	files {
		"Trident/vendor/tsl/tsl/**.cpp",
		"Trident/vendor/tsl/tsl/**.hpp",
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
		-- TODO(ci/vk/sdk-inclusion): Only include in debug mode, when the validation layers are enabled.
		VULKAN_SDK .. "/Include",
	}

	links {
		"Trident",
	}

	filter "_ACTION:vs2019 or _ACTION:vs2022 or _ACTION:vs2026"
		postbuildcommands {
			-- TODO(debug-only): Test LSP server executable inside IDE.
			-- "{COPYFILE} %[bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.exe] C:/Users/Mattéo/.vscode/extensions/tsl/src/lsp.exe", -- TODO: (debug-only)
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