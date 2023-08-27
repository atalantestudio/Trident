project "glfw"
	location "glfw"
	kind "StaticLib"
	language "C"

	targetdir ("%{prj.name}/bin/" .. outputdir)
	objdir ("%{prj.name}/obj/" .. outputdir)

	files {
		"%{prj.name}/%{prj.name}/include/GLFW/glfw3.h",
		"%{prj.name}/%{prj.name}/include/GLFW/glfw3native.h",
		"%{prj.name}/%{prj.name}/src/context.c",
		"%{prj.name}/%{prj.name}/src/glfw_config.h",
		"%{prj.name}/%{prj.name}/src/init.c",
		"%{prj.name}/%{prj.name}/src/input.c",
		"%{prj.name}/%{prj.name}/src/monitor.c",
		"%{prj.name}/%{prj.name}/src/vulkan.c",
		"%{prj.name}/%{prj.name}/src/window.c",
	}

	filter "system:windows"
		staticruntime "On"
		systemversion "latest"

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
			"%{prj.name}/%{prj.name}/src/win32_monitor.c",
			"%{prj.name}/%{prj.name}/src/win32_thread.c",
			"%{prj.name}/%{prj.name}/src/win32_time.c",
			"%{prj.name}/%{prj.name}/src/win32_window.c",
		}

	filter {"system:windows", "configurations:Release"}
		buildoptions "/MT"