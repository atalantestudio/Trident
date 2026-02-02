"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=vert VisibilityVK.vert.glsl -o Visibility.vert.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=frag Visibility.frag.glsl -o Visibility.frag.spv

"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=vert VisibilityDebugVK.vert.glsl -o VisibilityDebug.vert.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=frag VisibilityDebugVK.frag.glsl -o VisibilityDebug.frag.spv