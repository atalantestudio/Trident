@echo off

pushd %~dp0\..\

set dxcpath="vendor/dxc/dxc"
set glslcpath="vendor/glslc/glslc"

echo Building shaders in AssetBuilder/Asset/Shader.
call "AssetBuilder/Asset/Shader/Build.dxc" %dxcpath%
call "AssetBuilder/Asset/Shader/Build.glslc" %glslcpath%

echo Building shaders in Trident/src/Asset/Shader/Debug.
call "Trident/src/Asset/Shader/Debug/Build.dxc" %dxcpath%
call "Trident/src/Asset/Shader/Debug/Build.glslc" %glslcpath%

echo Building shaders in Trident/src/Asset/Shader/Forward.
call "Trident/src/Asset/Shader/Forward/Build.dxc" %dxcpath%
call "Trident/src/Asset/Shader/Forward/Build.glslc" %glslcpath%

echo Building shaders in Trident/src/Asset/Shader/GUI.
call "Trident/src/Asset/Shader/GUI/Build.dxc" %dxcpath%
call "Trident/src/Asset/Shader/GUI/Build.glslc" %glslcpath%

echo Building shaders in Trident/src/Asset/Shader/Material.
call "Trident/src/Asset/Shader/Material/Build.dxc" %dxcpath%
call "Trident/src/Asset/Shader/Material/Build.glslc" %glslcpath%

echo Building shaders in Trident/src/Asset/Shader/Visibility.
call "Trident/src/Asset/Shader/Visibility/Build.dxc" %dxcpath%
call "Trident/src/Asset/Shader/Visibility/Build.glslc" %glslcpath%

echo Building shaders in Samples/Asset/Shader/Compute.
call "Samples/Asset/Shader/Compute/Build.dxc" %dxcpath%
call "Samples/Asset/Shader/Compute/Build.glslc" %glslcpath%

echo Building shaders in Samples/Asset/Shader/Physics2D.
call "Samples/Asset/Shader/Physics2D/Build.dxc" %dxcpath%
call "Samples/Asset/Shader/Physics2D/Build.glslc" %glslcpath%

echo Building shaders in Samples/Asset/Shader/Sample.
call "Samples/Asset/Shader/Sample/Build.dxc" %dxcpath%
call "Samples/Asset/Shader/Sample/Build.glslc" %glslcpath%

echo Done.

popd