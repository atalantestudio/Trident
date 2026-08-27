@echo off

set glslcpath=%1
set inputpath=%~dp0

%glslcpath% -fshader-stage=vert "%inputpath%/Skybox.vert.glsl" -o "%inputpath%/Skybox.vert.spv"

%glslcpath% -fshader-stage=vert "%inputpath%/Geometry.vert.glsl" -o "%inputpath%/Geometry.vert.spv"
%glslcpath% -fshader-stage=frag "%inputpath%/Geometry.frag.glsl" -o "%inputpath%/Geometry.frag.spv"