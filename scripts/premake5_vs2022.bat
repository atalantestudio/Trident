@echo off

pushd %~dp0\..\

call "vendor/premake5/premake5.0.0-beta8-windows.exe" vs2022 --cc=msc-v143

popd