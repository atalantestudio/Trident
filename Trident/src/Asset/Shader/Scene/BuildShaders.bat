@echo off

set glslcpath=%1
set inputpath=%~dp0

%glslcpath% -fshader-stage=comp "%inputpath%/Trace.comp.glsl" -o "%inputpath%/Trace.comp.spv"