"%VULKAN_SDK%\Bin\glslc.exe" Texture.vert -o Texture.vert.spv
"%VULKAN_SDK%\Bin\glslc.exe" Texture.frag -o Texture.frag.spv

"%VULKAN_SDK%\Bin\glslc.exe" TextureArray.vert -o TextureArray.vert.spv
"%VULKAN_SDK%\Bin\glslc.exe" TextureArray.frag -o TextureArray.frag.spv

"%VULKAN_SDK%\Bin\glslc.exe" Glyph.vert -o Glyph.vert.spv
"%VULKAN_SDK%\Bin\glslc.exe" Glyph.frag -o Glyph.frag.spv

"%VULKAN_SDK%\Bin\glslc.exe" GlyphImage.comp -o GlyphImage.comp.spv
"%VULKAN_SDK%\Bin\glslc.exe" GlyphDistanceField.comp -o GlyphDistanceField.comp.spv