workspace "ProjectA"
	architecture "x86_64"
	startproject "Sandbox"

	configurations {
		"Debug",
		"Profile",
		"Release",
	}

	flags {
		"MultiProcessorCompile",
	}

outputdir = "%{cfg.system}_%{cfg.architecture}_%{cfg.buildcfg}"
vulkansdk = os.getenv("VULKAN_SDK")

group "Dependencies"
	include "ProjectA/vendor"

group ""

project "ProjectA"
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
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/vendor/lyah/lyah/**.hpp",
	}

	includedirs {
		"%{prj.name}/src",
		"%{prj.name}/vendor/lyah",
		vulkansdk .. "/Include",
	}

	libdirs {
		vulkansdk .. "/Lib",
	}

	links {
		"opengl32.lib",
		"vulkan-1.lib",
	}

	filter "system:windows"
		systemversion "latest"

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

project "Sandbox"
	location "%{prj.name}"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.hpp",
	}

	includedirs {
		"ProjectA/src",
		"ProjectA/vendor",
		"ProjectA/vendor/lyah",
		vulkansdk .. "/Include", -- TODO: Temporary
	}

	links {
		"ProjectA",
	}

	defines {
		"PROJECT_NAME=\"%{prj.name}\"",
	}

	postbuildcommands {
		"{COPYDIR} Asset %[bin/" .. outputdir .. "/%{prj.name}/Asset]",
		"{COPYFILE} %[%{prj.name}.tpr] %[bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.tpr]",
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

project "Editor"
	location "%{prj.name}"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("obj/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.hpp",
	}

	includedirs {
		"ProjectA/src",
		"ProjectA/vendor",
		"ProjectA/vendor/lyah",
		vulkansdk .. "/Include", -- TODO: Temporary
	}

	links {
		-- "opengl32.lib", -- TODO: Temporary
		"ProjectA",
		-- "vulkan-1.lib", -- TODO: Temporary
	}

	defines {
		"PROJECT_NAME=\"%{prj.name}\"",
	}

	postbuildcommands {
		"{COPYDIR} Asset %[bin/" .. outputdir .. "/%{prj.name}/Asset]",
		"{COPYFILE} %[%{prj.name}.tpr] %[bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.tpr]",
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