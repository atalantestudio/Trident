:: "%VULKAN_SDK%\Bin\glslc.exe" Base/Vertex2DRGB.vert -o Base/Vertex2DRGB.vert.spv
:: "%VULKAN_SDK%\Bin\glslc.exe" Base/Vertex2DRGBA.vert -o Base/Vertex2DRGBA.vert.spv
:: "%VULKAN_SDK%\Bin\glslc.exe" Base/Vertex2DUV.vert -o Base/Vertex2DUV.vert.spv
:: "%VULKAN_SDK%\Bin\glslc.exe" Base/Vertex3DRGB.vert -o Base/Vertex3DRGB.vert.spv
:: "%VULKAN_SDK%\Bin\glslc.exe" Base/Vertex3DRGBA.vert -o Base/Vertex3DRGBA.vert.spv

:: "%VULKAN_SDK%\Bin\glslc.exe" Base/ColorRGB.frag -o Base/ColorRGB.frag.spv
:: "%VULKAN_SDK%\Bin\glslc.exe" Base/ColorRGBA.frag -o Base/ColorRGBA.frag.spv
:: "%VULKAN_SDK%\Bin\glslc.exe" Base/TextureRGBA.frag -o Base/TextureRGBA.frag.spv

"%VULKAN_SDK%\Bin\glslc.exe" ArrayTextureOrTextureArray.vert -o ArrayTextureOrTextureArray.vert.spv

"%VULKAN_SDK%\Bin\glslc.exe" ArrayTexture.frag -o ArrayTexture.frag.spv

"%VULKAN_SDK%\Bin\glslc.exe" TextureArray.frag -o TextureArray.frag.spv

:: "%VULKAN_SDK%\Bin\glslc.exe" Glyph.vert -o Glyph.vert.spv
:: "%VULKAN_SDK%\Bin\glslc.exe" Glyph.frag -o Glyph.frag.spv

:: "%VULKAN_SDK%\Bin\glslc.exe" ImageUVGradient.comp -o ImageUVGradient.comp.spv