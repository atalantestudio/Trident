@echo off

call mingw32-make config=profile -j8

@REM NOTE
@REM Post-build commands are a manual step for Clang/GCC (Premake bundles them at the end of the compilation).
@REM However I've encountered issues with this step depending on the CLI used.
@REM For now, the post-build step is always executed as a .bat script when using these compilers.

pushd %~dp0\..\..\

set "outputdir=bin/windows_x86_64_Profile"

@REM If a .tpr file exists, use it as the default configuration file.
if exist "AssetBuilder/AssetBuilder.tpr" (
	cp "AssetBuilder/AssetBuilder.tpr" "%outputdir%/AssetBuilder/AssetBuilder.tpr"

	echo Copied AssetBuilder/AssetBuilder.tpr to %outputdir%/AssetBuilder/AssetBuilder.tpr.
)

@REM If a .tpr file exists, use it as the default configuration file.
if exist "Samples/Samples.tpr" (
	cp "Samples/Samples.tpr" "%outputdir%/Samples/Samples.tpr"

	echo Copied Samples/Samples.tpr to %outputdir%/Samples/Samples.tpr.
)

@REM Create asset directories.
if not exist "%outputdir%/Samples/Asset" mkdir "%outputdir%/Samples/Asset"
if not exist "%outputdir%/Samples/Asset/Font" mkdir "%outputdir%/Samples/Asset/Font"
if not exist "%outputdir%/Samples/Asset/Geometry" mkdir "%outputdir%/Samples/Asset/Geometry"
if not exist "%outputdir%/Samples/Asset/Image" mkdir "%outputdir%/Samples/Asset/Image"
if not exist "%outputdir%/Samples/Asset/Scene" mkdir "%outputdir%/Samples/Asset/Scene"
if not exist "%outputdir%/Samples/Asset/Shader" mkdir "%outputdir%/Samples/Asset/Shader"

@REM Create trace output directory.
if not exist "%outputdir%/Samples/Trace" mkdir "%outputdir%/Samples/Trace"

@REM Copy shaders.
if exist "Samples/Asset/Shader" (
	@REM TODO(ci/post-build-commands/hide-xcopy-message)
	@REM xcopy confirmation messages should be disabled with /q but they are not.
	@REM See https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/xcopy.
	xcopy "Samples/Asset/Shader" "%outputdir%/Samples/Asset/Shader" /e /y /i > nul

	echo Copied Samples/Asset/Shader to %outputdir%/Samples/Asset/Shader.
)

popd