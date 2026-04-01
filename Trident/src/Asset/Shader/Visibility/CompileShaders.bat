"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=vert Visibility.vert.glsl -o Visibility.vert.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=vert VisibilitySingleSet.vert.glsl -o VisibilitySingleSet.vert.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=frag Visibility.frag.glsl -o Visibility.frag.spv

@REM "%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=vert VisibilityDebug.vert.glsl -o VisibilityDebugGL.vert.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=vert VisibilityDebugVK.vert.glsl -o VisibilityDebug.vert.spv
@REM "%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=frag VisibilityDebug.frag.glsl -o VisibilityDebugGL.frag.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=frag VisibilityDebugVK.frag.glsl -o VisibilityDebug.frag.spv

"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=comp MaterialCoordinate.comp.glsl -o MaterialCoordinate.comp.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=comp MaterialCoordinateGL.comp.glsl -o MaterialCoordinateGL.comp.spv

"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=comp MaterialCount.comp.glsl -o MaterialCount.comp.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=comp MaterialCountGL.comp.glsl -o MaterialCountGL.comp.spv

"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=comp MaterialOffset.comp.glsl -o MaterialOffset.comp.spv

"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=vert Output.vert.glsl -o Output.vert.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=frag Output.frag.glsl -o Output.frag.spv

@REM Material shaders
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=comp Material/Default.glsl -o Material/Default.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=comp Material/Test.glsl -o Material/Test.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=comp Material/TestGL.glsl -o Material/TestGL.spv