outputdir = "%{cfg.system}_%{cfg.architecture}_%{cfg.buildcfg}"

workspace "ProjectA"
	architecture "x64"

	configurations {
		"Debug",
		"Profile",
		"Release",
	}

include "ProjectA/vendor"

project "ProjectA"
	location "ProjectA"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("bin/%{prj.name}/" .. outputdir)
	objdir ("obj/%{prj.name}/" .. outputdir)

	pchheader "pch.hpp"
	pchsource "%{prj.name}/src/pch.cpp"

	files {
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.hpp",
	}

	includedirs {
		"%{prj.name}/src",
		"%{prj.name}/vendor/glad/include",
		"%{prj.name}/vendor/glfw/glfw/include",
		"%{prj.name}/vendor/imgui",
		"%{prj.name}/vendor/spdlog/include",
	}

	links {
		"glad",
		"glfw",
		"imgui",
		"opengl32.lib",
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++latest"

		defines {
			"WINDOWS",
			"BUILD_DLL",
			"GLFW_INCLUDE_NONE",
			"IMGUI_IMPL_OPENGL_LOADER_CUSTOM",
		}

		postbuildcommands {
			("{COPY} %{cfg.buildtarget.relpath} ../bin/Sandbox/" .. outputdir),
		}

	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Profile"
		defines "PROFILE"
		runtime "Release"
		optimize "on"

	filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	targetdir ("bin/%{prj.name}/" .. outputdir)
	objdir ("obj/%{prj.name}/" .. outputdir)

	files {
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.hpp",
	}

	includedirs {
		"ProjectA/src",
		"ProjectA/vendor/spdlog/include",
	}

	links {
		"ProjectA",
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++latest"

		defines "WINDOWS"

	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Profile"
		defines "PROFILE"
		runtime "Release"
		optimize "on"

	filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
		optimize "on"