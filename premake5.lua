outputdir = "%{cfg.system}_%{cfg.architecture}_%{cfg.buildcfg}"

workspace "ProjectA"
	architecture "x64"

	configurations {
		"Debug",
		"Profile",
		"Release",
	}

project "ProjectA"
	location "ProjectA"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/%{prj.name}/" .. outputdir)
	objdir ("obj/%{prj.name}/" .. outputdir)

	files {
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.hpp",
	}

	includedirs {
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
	}

	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "On"
		systemversion "latest"

		defines {
			"WINDOWS",
			"BUILD_DLL",
		}

		postbuildcommands {
			("{COPY} %{cfg.buildtarget.relpath} ../bin/Sandbox/" .. outputdir),
		}

	filter "configurations:Debug"
		defines "DEBUG"
		symbols "On"

	filter "configurations:Profile"
		defines "PROFILE"
		optimize "On"

	filter "configurations:Release"
		defines "RELEASE"
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
		cppdialect "C++latest"
		staticruntime "On"
		systemversion "latest"

		defines "WINDOWS"

	filter "configurations:Debug"
		defines "DEBUG"
		symbols "On"

	filter "configurations:Profile"
		defines "PROFILE"
		optimize "On"

	filter "configurations:Release"
		defines "RELEASE"
		optimize "On"