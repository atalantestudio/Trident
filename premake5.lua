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
		"%{prj.name}/vendor/spdlog/include",
	}

	links {
		"glad",
		"glfw",
		"opengl32.lib",
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++latest"
		staticruntime "On"

		defines {
			"WINDOWS",
			"BUILD_DLL",
			"GLFW_INCLUDE_NONE",
		}

		postbuildcommands {
			("{COPY} %{cfg.buildtarget.relpath} ../bin/Sandbox/" .. outputdir),
		}

	filter "configurations:Debug"
		defines "DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Profile"
		defines "PROFILE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Release"
		defines "RELEASE"
		buildoptions "/MD"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

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
		staticruntime "On"

		defines "WINDOWS"

	filter "configurations:Debug"
		defines "DEBUG"
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Profile"
		defines "PROFILE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Release"
		defines "RELEASE"
		buildoptions "/MD"
		optimize "On"