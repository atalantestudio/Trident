project "glad"
	location "glad"
	kind "StaticLib"
	language "C"

	targetdir ("../../bin/ProjectA/" .. outputdir .. "/%{prj.name}")
	objdir ("../../obj/ProjectA/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/include/glad/glad.h",
		"%{prj.name}/include/KHR/khrplatform.h",
		"%{prj.name}/src/glad.c",
	}

	includedirs {
		"%{prj.name}/include",
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

	filter {"system:windows", "configurations:Release"}
		buildoptions "/MT"

project "glfw"
	location "glfw"
	kind "StaticLib"
	language "C"

	targetdir ("../../bin/ProjectA/" .. outputdir .. "/%{prj.name}")
	objdir ("../../obj/ProjectA/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/%{prj.name}/include/GLFW/glfw3.h",
		"%{prj.name}/%{prj.name}/include/GLFW/glfw3native.h",
		"%{prj.name}/%{prj.name}/src/glfw_config.h",
		"%{prj.name}/%{prj.name}/src/internal.h",
		"%{prj.name}/%{prj.name}/src/context.c",
		"%{prj.name}/%{prj.name}/src/init.c",
		"%{prj.name}/%{prj.name}/src/input.c",
		"%{prj.name}/%{prj.name}/src/monitor.c",
		"%{prj.name}/%{prj.name}/src/null_init.c",
		"%{prj.name}/%{prj.name}/src/null_joystick.c",
		"%{prj.name}/%{prj.name}/src/null_monitor.c",
		"%{prj.name}/%{prj.name}/src/null_window.c",
		"%{prj.name}/%{prj.name}/src/platform.c",
		"%{prj.name}/%{prj.name}/src/vulkan.c",
		"%{prj.name}/%{prj.name}/src/window.c",
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

		defines {
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS",
		}

		files {
			"%{prj.name}/%{prj.name}/src/egl_context.c",
			"%{prj.name}/%{prj.name}/src/osmesa_context.c",
			"%{prj.name}/%{prj.name}/src/wgl_context.c",
			"%{prj.name}/%{prj.name}/src/win32_init.c",
			"%{prj.name}/%{prj.name}/src/win32_joystick.c",
			"%{prj.name}/%{prj.name}/src/win32_module.c",
			"%{prj.name}/%{prj.name}/src/win32_monitor.c",
			"%{prj.name}/%{prj.name}/src/win32_thread.c",
			"%{prj.name}/%{prj.name}/src/win32_time.c",
			"%{prj.name}/%{prj.name}/src/win32_window.c",
		}

	filter {"system:windows", "configurations:Release"}
		buildoptions "/MT"

project "imgui"
	location "imgui"
	kind "StaticLib"
	language "C++"

	targetdir ("../../bin/ProjectA/" .. outputdir .. "/%{prj.name}")
	objdir ("../../obj/ProjectA/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/imconfig.h",
		"%{prj.name}/imgui.h",
		"%{prj.name}/imgui.cpp",
		"%{prj.name}/imgui_draw.cpp",
		"%{prj.name}/imgui_interval.h",
		"%{prj.name}/imgui_tables.cpp",
		"%{prj.name}/imgui_widgets.cpp",
		"%{prj.name}/imstb_rectpack.h",
		"%{prj.name}/imstb_textedit.h",
		"%{prj.name}/imstb_truetype.h",
		"%{prj.name}/imgui_demo.cpp",
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

	filter {"system:windows", "configurations:Release"}
		buildoptions "/MT"