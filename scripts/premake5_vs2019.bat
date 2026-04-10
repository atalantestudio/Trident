@echo off

pushd %~dp0\..\

call "vendor/premake5/premake5.0.0-beta8-windows" vs2019 --cc=msc-v142

popd