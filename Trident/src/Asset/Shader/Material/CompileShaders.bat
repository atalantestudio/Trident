"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=frag PBR.frag.glsl -o PBR.frag.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=frag PBRSingleSet.frag.glsl -o PBRSingleSet.frag.spv

"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=frag PBRSeparatedAORoughness.frag.glsl -o PBRSeparatedAORoughness.frag.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=frag PBRSeparatedAORoughnessSingleSet.frag.glsl -o PBRSeparatedAORoughnessSingleSet.frag.spv