project "Samples"
	location "."
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	vectorextensions ("AVX2", "AVX", "SSE4.1", "SSE3", "SSE2", "SSE")

	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../obj/" .. outputdir .. "/%{prj.name}")

	dependson {
		"Trident",
	}

	files {
		"src/**.cpp",
		"src/**.hpp",
	}

	includedirs {
		"../Trident/src",
		"../Trident/vendor/API",
		"../Trident/vendor/Base",
		"../Trident/vendor/lyah",
		"../Trident/vendor/scroll",
		"../Trident/vendor/tsl",
		vulkansdk .. "/Include", -- TODO: Temporary
	}

	links {
		"Trident",
	}

	defines {
		"PROJECT_NAME=\"%{prj.name}\"",
	}

	filter "system:windows"
		systemversion "latest"

	filter "_ACTION:vs2019 or _ACTION:vs2022 or _ACTION:vs2026"
		postbuildcommands {
			-- Create asset directories.
			"{MKDIR} %[../bin/" .. outputdir .. "/%{prj.name}/Asset]",
			"{MKDIR} %[../bin/" .. outputdir .. "/%{prj.name}/Asset/Font]",
			"{MKDIR} %[../bin/" .. outputdir .. "/%{prj.name}/Asset/Geometry]",
			"{MKDIR} %[../bin/" .. outputdir .. "/%{prj.name}/Asset/Image]",
			"{MKDIR} %[../bin/" .. outputdir .. "/%{prj.name}/Asset/Scene]",
			"{MKDIR} %[../bin/" .. outputdir .. "/%{prj.name}/Asset/Shader]",
			"{MKDIR} %[../bin/" .. outputdir .. "/%{prj.name}/Trace]",

			-- Copy shaders.
			-- NOTE: IF EXIST required on cmd.exe. See https://premake.github.io/docs/Tokens#command-tokens.
			"IF EXIST Asset/Shader {COPYDIR} Asset/Shader %[../bin/" .. outputdir .. "/%{prj.name}/Asset/Shader]",

			-- If a .tpr file exists, use it as the default configuration file.
			-- NOTE: IF EXIST required on cmd.exe. See https://premake.github.io/docs/Tokens#command-tokens.
			"IF EXIST %[%{prj.name}.tpr] {COPYFILE} %[%{prj.name}.tpr] %[../bin/" .. outputdir .. "/%{prj.name}/%{prj.name}.tpr]",
		}

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