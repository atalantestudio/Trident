@echo off

pushd %~dp0\..\..\
call vendor\premake5\premake5.exe vs2022
popd
pause