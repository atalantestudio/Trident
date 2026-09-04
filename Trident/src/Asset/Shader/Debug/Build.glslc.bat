@echo off

set glslcpath=%1
set inputpath=%~dp0

%glslcpath% -fshader-stage=vert "%inputpath%/Skybox.vert.glsl" -o "%inputpath%/Skybox.vert.spv"
%glslcpath% -fshader-stage=vert "%inputpath%/GeometryFill.vert.glsl" -o "%inputpath%/GeometryFill.vert.spv"
%glslcpath% -fshader-stage=frag "%inputpath%/GeometryFill.frag.glsl" -o "%inputpath%/GeometryFill.frag.spv"
%glslcpath% -fshader-stage=vert "%inputpath%/GeometryLine.vert.glsl" -o "%inputpath%/GeometryLine.vert.spv"
%glslcpath% -fshader-stage=frag "%inputpath%/GeometryLine.frag.glsl" -o "%inputpath%/GeometryLine.frag.spv"