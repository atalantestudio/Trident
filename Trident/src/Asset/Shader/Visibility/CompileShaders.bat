"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=vert Visibility.vert.glsl -o Visibility.vert.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=vert VisibilitySingleSet.vert.glsl -o VisibilitySingleSet.vert.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=frag Visibility.frag.glsl -o Visibility.frag.spv

"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=comp VisibilityDebug.comp.glsl -o VisibilityDebug.comp.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=comp VisibilityDebugGL.comp.glsl -o VisibilityDebugGL.comp.spv

"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=comp MaterialCoordinate.comp.glsl -o MaterialCoordinate.comp.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=comp MaterialCoordinateGL.comp.glsl -o MaterialCoordinateGL.comp.spv

"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=comp MaterialCount.comp.glsl -o MaterialCount.comp.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=comp MaterialCountGL.comp.glsl -o MaterialCountGL.comp.spv

"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=comp MaterialOffset.comp.glsl -o MaterialOffset.comp.spv

"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=vert Output.vert.glsl -o Output.vert.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=frag Output.frag.glsl -o Output.frag.spv