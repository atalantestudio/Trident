workspace "ProjectA"
	architecture "x86_64"
	startproject "Samples"

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

-- Checkout submodules.
os.execute("git config --global --add safe.directory *")
os.execute("git submodule update --init")

--[[ os.mkdir("ProjectA/vendor/API/GL")
os.mkdir("ProjectA/vendor/API/KHR")

-- Download required files from the OpenGL Registry.
local glcorearbresult = http.download("https://registry.khronos.org/OpenGL/api/GL/glcorearb.h", "ProjectA/vendor/API/GL/glcorearb.h")
local glextresult = http.download("https://registry.khronos.org/OpenGL/api/GL/glext.h", "ProjectA/vendor/API/GL/glext.h")
local wglextresult = http.download("https://registry.khronos.org/OpenGL/api/GL/wglext.h", "ProjectA/vendor/API/GL/wglext.h")
local khrplatformresult = http.download("https://raw.githubusercontent.com/KhronosGroup/EGL-Registry/refs/heads/main/api/KHR/khrplatform.h", "ProjectA/vendor/API/KHR/khrplatform.h")

printf("Fetching glcorearb.h: %s", glcorearbresult)
printf("Fetching glext.h: %s", glextresult)
printf("Fetching wglext.h: %s", wglextresult)
printf("Fetching khrplatform.h: %s", khrplatformresult) ]]--

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
		"%{prj.name}/vendor/API/**.h",
		"%{prj.name}/vendor/lyah/lyah/**.hpp",
	}

	includedirs {
		"%{prj.name}/src",
		"%{prj.name}/vendor/API",
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

project "Samples"
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
		"ProjectA/vendor/API",
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
		"MKDIR %[bin/" .. outputdir .. "/%{prj.name}/Asset/Font]",
		"MKDIR %[bin/" .. outputdir .. "/%{prj.name}/Asset/Image]",
		"MKDIR %[bin/" .. outputdir .. "/%{prj.name}/Asset/Shader]",
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

	files {
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.hpp",
	}

	includedirs {
		"ProjectA/src",
		"ProjectA/vendor",
		"ProjectA/vendor/API",
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