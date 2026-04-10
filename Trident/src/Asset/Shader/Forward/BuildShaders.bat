@echo off

set glslcpath=%1
set inputpath=%~dp0

%glslcpath% -fshader-stage=vert "%inputpath%/Forward.vert.glsl" -o "%inputpath%/Forward.vert.spv"