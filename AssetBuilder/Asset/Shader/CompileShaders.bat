"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=comp DistanceField.comp.glsl -o DistanceField.comp.spv

"%VULKAN_SDK%\Bin\glslc.exe" -fshader-stage=comp RasterizeGlyph.comp.glsl -o RasterizeGlyph.comp.spv