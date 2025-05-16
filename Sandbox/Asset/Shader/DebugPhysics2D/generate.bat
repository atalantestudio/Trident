:: "%VULKAN_SDK%\Bin\glslc.exe" Grid.vert -o Grid.vert.spv
:: "%VULKAN_SDK%\Bin\glslc.exe" Grid.frag -o Grid.frag.spv

:: "%VULKAN_SDK%\Bin\glslc.exe" Fill.vert -o Fill.vert.spv
:: "%VULKAN_SDK%\Bin\glslc.exe" Fill.frag -o Fill.frag.spv

:: "%VULKAN_SDK%\Bin\glslc.exe" Stroke.vert -o Stroke.vert.spv
:: "%VULKAN_SDK%\Bin\glslc.exe" Stroke.frag -o Stroke.frag.spv

"%VULKAN_SDK%\Bin\glslc.exe" CenterOfMass.vert -o CenterOfMass.vert.spv
"%VULKAN_SDK%\Bin\glslc.exe" CenterOfMass.frag -o CenterOfMass.frag.spv

:: "%VULKAN_SDK%\Bin\glslc.exe" VertexFeature.vert -o VertexFeature.vert.spv
:: "%VULKAN_SDK%\Bin\glslc.exe" VertexFeature.frag -o VertexFeature.frag.spv

:: "%VULKAN_SDK%\Bin\glslc.exe" EdgeFeature.vert -o EdgeFeature.vert.spv
:: "%VULKAN_SDK%\Bin\glslc.exe" EdgeFeature.frag -o EdgeFeature.frag.spv