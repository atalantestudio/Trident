@echo off

pushd %~dp0\..\

set glslcpath="vendor/glslc/glslc"

echo Building shaders in AssetBuilder/Asset/Shader.
call "AssetBuilder/Asset/Shader/BuildShaders" %glslcpath%

echo Building shaders in Trident/src/Asset/Shader/Forward.
call "Trident/src/Asset/Shader/Forward/BuildShaders" %glslcpath%

echo Building shaders in Trident/src/Asset/Shader/GUI.
call "Trident/src/Asset/Shader/GUI/BuildShaders" %glslcpath%

echo Building shaders in Trident/src/Asset/Shader/Material.
call "Trident/src/Asset/Shader/Material/BuildShaders" %glslcpath%

echo Building shaders in Trident/src/Asset/Shader/Scene.
call "Trident/src/Asset/Shader/Scene/BuildShaders" %glslcpath%

echo Building shaders in Trident/src/Asset/Shader/Visibility.
call "Trident/src/Asset/Shader/Visibility/BuildShaders" %glslcpath%

echo Building shaders in Samples/Asset/Shader/Base.
call "Samples/Asset/Shader/Base/BuildShaders" %glslcpath%

echo Building shaders in Samples/Asset/Shader/Compute.
call "Samples/Asset/Shader/Compute/BuildShaders" %glslcpath%

echo Building shaders in Samples/Asset/Shader/Physics2D.
call "Samples/Asset/Shader/Physics2D/BuildShaders" %glslcpath%

echo Building shaders in Samples/Asset/Shader/Sample.
call "Samples/Asset/Shader/Sample/BuildShaders" %glslcpath%

popd