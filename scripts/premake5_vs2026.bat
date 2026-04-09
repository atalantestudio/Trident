@echo off

pushd %~dp0\..\

call "vendor/premake5/premake5.0.0-beta8-windows.exe" vs2026 --cc=msc-v145

popd