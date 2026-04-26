project "Samples"
	location "."
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	vectorextensions "AVX2"

	targetdir ("../bin/" .. OUTPUT_DIR .. "/%{prj.name}")
	objdir ("../obj/" .. OUTPUT_DIR .. "/%{prj.name}")

	dependson {
		"Trident",
	}

	files {
		"src/**.cpp",
		"src/**.hpp",
	}

	includedirs {
		VULKAN_SDK .. "/Include",
		"../Trident/src",
		"../Trident/vendor/API",
		"../Trident/vendor/Base",
		"../Trident/vendor/lyah",
		"../Trident/vendor/scroll",
		"../Trident/vendor/tsl",
	}

	links {
		"Trident",
	}

	defines {
		"PROJECT_NAME=\"%{prj.name}\"",
	}

	filter "toolset:msc-v142 or toolset:msc-v143 or toolset:msc-v145"
		postbuildcommands {
			-- Create asset directories.
			"{MKDIR} %[../bin/" .. OUTPUT_DIR .. "/%{prj.name}/Asset]",
			"{MKDIR} %[../bin/" .. OUTPUT_DIR .. "/%{prj.name}/Asset/Font]",
			"{MKDIR} %[../bin/" .. OUTPUT_DIR .. "/%{prj.name}/Asset/Geometry]",
			"{MKDIR} %[../bin/" .. OUTPUT_DIR .. "/%{prj.name}/Asset/Image]",
			"{MKDIR} %[../bin/" .. OUTPUT_DIR .. "/%{prj.name}/Asset/Scene]",
			"{MKDIR} %[../bin/" .. OUTPUT_DIR .. "/%{prj.name}/Asset/Shader]",

			-- Copy shaders.
			-- NOTE: IF EXIST required on cmd.exe. See https://premake.github.io/docs/Tokens#command-tokens.
			"IF EXIST Asset/Shader {COPYDIR} Asset/Shader %[../bin/" .. OUTPUT_DIR .. "/%{prj.name}/Asset/Shader]",

			-- If a .tpr file exists, use it as the default configuration file.
			-- NOTE: IF EXIST required on cmd.exe. See https://premake.github.io/docs/Tokens#command-tokens.
			"IF EXIST %[%{prj.name}.tpr] {COPYFILE} %[%{prj.name}.tpr] %[../bin/" .. OUTPUT_DIR .. "/%{prj.name}/%{prj.name}.tpr]",
		}

	filter "configurations:Debug"
		defines {
			"BUILD_MODE_DEBUG",
		}

		runtime "Debug"
		symbols "on"
		optimize "off"

	filter "configurations:Profile"
		defines {
			"BUILD_MODE_PROFILE",
		}

		runtime "Release"
		optimize "on"

		postbuildcommands {
			-- Create trace output directory.
			"{MKDIR} %[../bin/" .. OUTPUT_DIR .. "/%{prj.name}/Trace]",
		}

	filter "configurations:Release"
		defines {
			"BUILD_MODE_RELEASE",
		}

		runtime "Release"
		optimize "on"