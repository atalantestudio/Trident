@echo off

pushd %~dp0\..\

@REM NOTE: clean has not yet been ported. See https://github.com/premake/premake-core/issues/278.
@REM call vendor\premake5\premake5.0.0-beta8-windows.exe clean

rm -rf bin
rm -rf obj

rm -rf **/Makefile
rm -rf **.make
rm -rf **.sln
rm -rf **.slnx
rm -rf **.vcxproj
rm -rf **.vcxproj.filters
rm -rf **.vcxproj.user

echo Cleaned output directories and configuration files.

popd