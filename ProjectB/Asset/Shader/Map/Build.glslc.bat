@echo off

set inputpath=%~dp0

glslc -fshader-stage=vert "%inputpath%/StaticGeometry.vert.glsl" -o "%inputpath%/StaticGeometry.vert.spv"
glslc -fshader-stage=frag "%inputpath%/StaticGeometry.frag.glsl" -o "%inputpath%/StaticGeometry.frag.spv"