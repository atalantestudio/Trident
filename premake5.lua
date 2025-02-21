workspace "ProjectA"
	architecture "x86_64"
	startproject "Editor"

	configurations {
		"Debug",
		"Profile",
		"Release",
	}

	flags {
		"MultiProcessorCompile",
	}

outputdir = "%{cfg.system}_%{cfg.architecture}_%{cfg.buildcfg}"

group "Dependencies"
	include "ProjectA/vendor"

group ""

project "ProjectA"
	location "%{prj.name}"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/%{prj.name}/" .. outputdir)
	objdir ("obj/%{prj.name}/" .. outputdir)

	pchheader "pch.hpp"
	pchsource "%{prj.name}/src/pch.cpp"

	defines {
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
	}

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

		excludes "%{prj.name}/src/%{prj.name}/Logger/Logger.cpp"

	filter "configurations:Release"
		defines "BUILD_MODE_RELEASE"
		runtime "Release"
		optimize "on"

		excludes "%{prj.name}/src/%{prj.name}/Logger/Logger.cpp"

project "Sandbox"
	location "%{prj.name}"
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

project "Editor"
	location "%{prj.name}"
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