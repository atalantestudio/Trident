workspace "Trident"
	architecture "x86_64"
	startproject "Samples"

	configurations {
		"Debug",
		"Profile",
		"Release",
	}

	multiprocessorcompile "on"

outputdir = "%{cfg.system}_%{cfg.architecture}_%{cfg.buildcfg}"
vulkansdk = os.getenv("VULKAN_SDK")

project "Trident"
	location "%{prj.name}"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.hpp"
	pchsource "%{prj.name}/src/pch.cpp"

	defines {
		"_CRT_SECURE_NO_WARNINGS",
	}

	files {
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/API/GL/glcorearb.h",
		"%{prj.name}/vendor/API/GL/glext.h",
		"%{prj.name}/vendor/API/GL/wglext.h",
		"%{prj.name}/vendor/API/KHR/khrplatform.h",
		"%{prj.name}/vendor/Base/Base/**.hpp",
		"%{prj.name}/vendor/lyah/lyah/**.hpp",
		"%{prj.name}/vendor/scroll/scroll/**.hpp",
		"%{prj.name}/vendor/tsl/tsl/**.cpp",
		"%{prj.name}/vendor/tsl/tsl/**.hpp",
	}

	removefiles {
		"%{prj.name}/src/Engine/Adapter/Graphics/DX12/**.cpp",
		-- "%{prj.name}/src/Engine/Adapter/Graphics/GL/**.cpp",
		-- "%{prj.name}/src/Engine/Adapter/Graphics/VK/**.cpp",
		"%{prj.name}/src/Engine/Adapter/Window/Win32/**.cpp",

		"%{prj.name}/vendor/tsl/tsl/tslc.cpp",
	}

	includedirs {
		"%{prj.name}/src",
		"%{prj.name}/vendor/API",
		"%{prj.name}/vendor/Base",
		"%{prj.name}/vendor/lyah",
		"%{prj.name}/vendor/scroll",
		"%{prj.name}/vendor/tsl",
		vulkansdk .. "/Include",
	}

	libdirs {
		vulkansdk .. "/Lib",
	}

	filter "system:windows"
		systemversion "latest"

		files {
			"%{prj.name}/src/Engine/Adapter/Graphics/DX12/**.cpp",
			"%{prj.name}/src/Engine/Adapter/Window/Win32/**.cpp",
		}

		links {
			-- "d3d12.lib",
			-- "d3dcompiler.lib",
			-- "dxgi.lib",
		}

	filter "configurations:Debug"
		defines {
			"BUILD_MODE_DEBUG",
		}

		runtime "Debug"
		symbols "on"

		excludes "%{prj.name}/src/Engine/Profile/**"

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

		excludes "%{prj.name}/src/Engine/Profile/**"

project "Samples"
	location "%{prj.name}"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")

	dependson {
		"Trident",
	}

	files {
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.hpp",
	}

	includedirs {
		"Trident/src",
		"Trident/vendor/API",
		"Trident/vendor/Base",
		"Trident/vendor/lyah",
		"Trident/vendor/scroll",
		"Trident/vendor/tsl",
		vulkansdk .. "/Include", -- TODO: Temporary
	}

	links {
		"Trident",
	}

	defines {
		"PROJECT_NAME=\"%{prj.name}\"",
	}

	postbuildcommands {
		"MKDIR %[bin/" .. outputdir .. "/%{prj.name}/Asset/Font]",
		"MKDIR %[bin/" .. outputdir .. "/%{prj.name}/Asset/Geometry]",
		"MKDIR %[bin/" .. outputdir .. "/%{prj.name}/Asset/Image]",
		"MKDIR %[bin/" .. outputdir .. "/%{prj.name}/Asset/Shader]",
		"MKDIR %[bin/" .. outputdir .. "/%{prj.name}/Trace]",
		"IF EXIST Asset/Shader {COPYDIR} Asset/Shader %[bin/" .. outputdir .. "/%{prj.name}/Asset/Shader]",
		"IF EXIST %[%{prj.name}.tpr] {COPYFILE} %[%{prj.name}.tpr] %[bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.tpr]",
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

project "AssetBuilder"
	location "%{prj.name}"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")

	dependson {
		"Trident",
	}

	files {
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.hpp",
	}

	includedirs {
		"Trident/src",
		"Trident/vendor/API",
		"Trident/vendor/Base",
		"Trident/vendor/lyah",
		"Trident/vendor/scroll",
		"Trident/vendor/tsl",
		vulkansdk .. "/Include", -- TODO: Temporary
	}

	links {
		"Trident",
	}

	defines {
		"PROJECT_NAME=\"%{prj.name}\"",
	}

	postbuildcommands {
		"IF EXIST %[%{prj.name}.tpr] {COPYFILE} %[%{prj.name}.tpr] %[bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.tpr]",
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

project "tslc"
	location "Trident/vendor/tsl"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

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
	cppdialect "C++17"
	staticruntime "on"

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

	postbuildcommands {
		"{COPYFILE} %[bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.exe] C:/Users/Mattéo/.vscode/extensions/tsl/src/lsp.exe", -- TODO: (debug-only)
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