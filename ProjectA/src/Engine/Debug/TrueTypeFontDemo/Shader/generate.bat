"%VULKAN_SDK%\Bin\glslc.exe" main.vert -o main.vert.spv
"%VULKAN_SDK%\Bin\glslc.exe" main.frag -o main.frag.spv
"%VULKAN_SDK%\Bin\glslc.exe" GlyphImage.comp -o GlyphImage.comp.spv
"%VULKAN_SDK%\Bin\glslc.exe" GlyphDistanceField.comp -o GlyphDistanceField.comp.spv