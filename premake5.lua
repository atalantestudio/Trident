VULKAN_SDK = os.getenv("VULKAN_SDK")
OUTPUT_DIR = "%{cfg.system}_%{cfg.architecture}_%{cfg.buildcfg}"

workspace "Trident"
	architecture "x86_64"
	startproject "Samples"
	multiprocessorcompile "on"

	configurations {
		"Debug",
		"Profile",
		"Release",
	}

	buildoptions {
		-- "-Wall",
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
			-- "-Wextra",
			"-Wno-unused-parameter",
		}

		filter "toolset:clang"
			buildoptions {
				"-Wno-missing-designated-field-initializers",
			}

			linkoptions {
				"-g",
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
	vectorextensions "AVX2"

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
		VULKAN_SDK .. "/Include",
		"Trident/src",
		"Trident/vendor/API",
		"Trident/vendor/Base",
		"Trident/vendor/lyah",
		"Trident/vendor/scroll",
		"Trident/vendor/tsl",
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

project "tsllsp"
	location "Trident/vendor/tsl"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	vectorextensions "AVX2"

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
		VULKAN_SDK .. "/Include",
		"Trident/src",
		"Trident/vendor/API",
		"Trident/vendor/Base",
		"Trident/vendor/lyah",
		"Trident/vendor/scroll",
		"Trident/vendor/tsl",
	}

	links {
		"Trident",
	}

	filter "toolset:msc-v142 or toolset:msc-v143 or toolset:msc-v145"
		postbuildcommands {
			-- TODO(debug-only): Test LSP server executable inside IDE.
			-- "{COPYFILE} %[bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.exe] C:/Users/Mattéo/.vscode/extensions/tsl/src/lsp.exe",
		}

	filter "system:windows"
		systemversion "latest"

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