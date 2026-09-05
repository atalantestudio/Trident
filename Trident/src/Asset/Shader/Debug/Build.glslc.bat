@echo off

set inputpath=%~dp0

glslc -fshader-stage=vert "%inputpath%/Skybox.vert.glsl" -o "%inputpath%/Skybox.vert.spv"
glslc -fshader-stage=vert "%inputpath%/GeometryFill.vert.glsl" -o "%inputpath%/GeometryFill.vert.spv"
glslc -fshader-stage=frag "%inputpath%/GeometryFill.frag.glsl" -o "%inputpath%/GeometryFill.frag.spv"
glslc -fshader-stage=vert "%inputpath%/GeometryLine.vert.glsl" -o "%inputpath%/GeometryLine.vert.spv"
glslc -fshader-stage=frag "%inputpath%/GeometryLine.frag.glsl" -o "%inputpath%/GeometryLine.frag.spv"