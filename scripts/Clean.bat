@echo off

pushd %~dp0\..\

@REM NOTE: clean has not yet been ported. See https://github.com/premake/premake-core/issues/278.
@REM call vendor\premake5\premake5.0.0-beta8-windows.exe clean

rm -rf bin
rm -rf obj

rm -rf Makefile **/Makefile
rm -rf **.make **/**.make
rm -rf **.sln **/**.sln
rm -rf **.slnx **/**.slnx
rm -rf **.vcxproj **/**.vcxproj
rm -rf **.vcxproj.filters **/**.vcxproj.filters
rm -rf **.vcxproj.user **/**.vcxproj.user

echo Cleaned output directories and configuration files.

popd