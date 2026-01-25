"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=vert Subcomponent.vert.glsl -o Subcomponent.vert.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=frag Subcomponent.frag.glsl -o Subcomponent.frag.spv

"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=vert GlyphSubcomponent.vert.glsl -o GlyphSubcomponent.vert.spv
"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=frag GlyphSubcomponent.frag.glsl -o GlyphSubcomponent.frag.spv