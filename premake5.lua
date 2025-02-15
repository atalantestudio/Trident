outputdir = "%{cfg.system}_%{cfg.architecture}_%{cfg.buildcfg}"

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

include "ProjectA/vendor"

project "ProjectA"
	location "ProjectA"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/%{prj.name}/" .. outputdir)
	objdir ("obj/%{prj.name}/" .. outputdir)

	pchheader "pch.hpp"
	pchsource "%{prj.name}/src/pch.cpp"

	files {
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/vendor/lyah/lyah/**.hpp",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
	}

	includedirs {
		"%{prj.name}/src",
		"%{prj.name}/vendor/glad/include",
		"%{prj.name}/vendor/glfw/glfw/include",
		"%{prj.name}/vendor/imgui",
		"%{prj.name}/vendor/lyah",
		"%{prj.name}/vendor/stb_image",
	}

	links {
		"glad",
		"GLFW",
		"imgui",
		"opengl32.lib",
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS",
	}

	filter "system:windows"
		systemversion "latest"

		defines {
			"BUILD_DLL",
			"GLFW_INCLUDE_NONE",
		}

	filter "configurations:Debug"
		defines "BUILD_MODE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Profile"
		defines "BUILD_MODE_PROFILE"
		runtime "Release"
		optimize "on"

	filter "configurations:Release"
		defines "BUILD_MODE_RELEASE"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/%{prj.name}/" .. outputdir)
	objdir ("obj/%{prj.name}/" .. outputdir)

	files {
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.hpp",
	}

	includedirs {
		"ProjectA/src",
		"ProjectA/vendor",
		"ProjectA/vendor/lyah",
	}

	links {
		"ProjectA",
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "BUILD_MODE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Profile"
		defines "BUILD_MODE_PROFILE"
		runtime "Release"
		optimize "on"

	filter "configurations:Release"
		defines "BUILD_MODE_RELEASE"
		runtime "Release"
		optimize "on"