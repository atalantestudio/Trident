@echo off

pushd %~dp0\..\

echo Building shaders in AssetBuilder/Asset/Shader.

call "AssetBuilder/Asset/Shader/Build.dxc"
call "AssetBuilder/Asset/Shader/Build.glslc"

echo Building shaders in Trident/src/Asset/Shader/Debug.

call "Trident/src/Asset/Shader/Debug/Build.dxc"
call "Trident/src/Asset/Shader/Debug/Build.glslc"

echo Building shaders in Trident/src/Asset/Shader/Forward.

call "Trident/src/Asset/Shader/Forward/Build.dxc"
call "Trident/src/Asset/Shader/Forward/Build.glslc"

echo Building shaders in Trident/src/Asset/Shader/GUI.

call "Trident/src/Asset/Shader/GUI/Build.dxc"
call "Trident/src/Asset/Shader/GUI/Build.glslc"

echo Building shaders in Trident/src/Asset/Shader/Material.

call "Trident/src/Asset/Shader/Material/Build.dxc"
call "Trident/src/Asset/Shader/Material/Build.glslc"

echo Building shaders in Trident/src/Asset/Shader/Visibility.

call "Trident/src/Asset/Shader/Visibility/Build.dxc"
call "Trident/src/Asset/Shader/Visibility/Build.glslc"

echo Building shaders in Samples/Asset/Shader/Compute.

call "Samples/Asset/Shader/Compute/Build.dxc"
call "Samples/Asset/Shader/Compute/Build.glslc"

echo Building shaders in Samples/Asset/Shader/Experimental.

call "Samples/Asset/Shader/Experimental/Build.glslc"

echo Building shaders in Samples/Asset/Shader/Physics2D.

call "Samples/Asset/Shader/Physics2D/Build.dxc"
call "Samples/Asset/Shader/Physics2D/Build.glslc"

echo Building shaders in Samples/Asset/Shader/Sample.

call "Samples/Asset/Shader/Sample/Build.dxc"
call "Samples/Asset/Shader/Sample/Build.glslc"

echo Done.

popd