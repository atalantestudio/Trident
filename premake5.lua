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
vulkansdk = "C:/VulkanSDK/1.3.296.0" -- TODO: Temporary path

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
		vulkansdk .. "/Include",
	}

	libdirs {
		vulkansdk .. "/Lib",
	}

	links {
		"glad",
		"glfw",
		"imgui",
		"opengl32.lib",
		"vulkan-1.lib",
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "BUILD_MODE_DEBUG"
		runtime "Debug"
		symbols "on"

		excludes "%{prj.name}/src/Engine/Profile/**"

	filter "configurations:Profile"
		defines "BUILD_MODE_PROFILE"
		runtime "Release"
		optimize "on"

		excludes "%{prj.name}/src/Engine/Logger/**"

	filter "configurations:Release"
		defines "BUILD_MODE_RELEASE"
		runtime "Release"
		optimize "on"

		excludes "%{prj.name}/src/Engine/Logger/**"
		excludes "%{prj.name}/src/Engine/Profile/**"

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
		"ProjectA/vendor/glfw/glfw/include", -- TODO: Temporary
		"ProjectA/src",
		"ProjectA/vendor",
		"ProjectA/vendor/lyah",
		vulkansdk .. "/Include", -- TODO: Temporary
	}

	libdirs {
		vulkansdk .. "/Lib", -- TODO: Temporary
	}

	links {
		"glfw", -- TODO: Temporary
		"opengl32.lib", -- TODO: Temporary
		"ProjectA",
		"vulkan-1.lib", -- TODO: Temporary
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