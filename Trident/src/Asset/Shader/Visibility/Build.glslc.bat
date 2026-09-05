@echo off

set inputpath=%~dp0

glslc -fshader-stage=vert "%inputpath%/Visibility.vert.glsl" -o "%inputpath%/Visibility.vert.spv"
glslc -fshader-stage=frag "%inputpath%/Visibility.frag.glsl" -o "%inputpath%/Visibility.frag.spv"
glslc -fshader-stage=comp "%inputpath%/VisibilityDebug.comp.glsl" -o "%inputpath%/VisibilityDebug.comp.spv"
glslc -fshader-stage=comp "%inputpath%/MaterialCoordinate.comp.glsl" -o "%inputpath%/MaterialCoordinate.comp.spv"
glslc -fshader-stage=comp "%inputpath%/MaterialCount.comp.glsl" -o "%inputpath%/MaterialCount.comp.spv"
glslc -fshader-stage=comp "%inputpath%/MaterialOffset.comp.glsl" -o "%inputpath%/MaterialOffset.comp.spv"
glslc -fshader-stage=vert "%inputpath%/Output.vert.glsl" -o "%inputpath%/Output.vert.spv"
glslc -fshader-stage=frag "%inputpath%/Output.frag.glsl" -o "%inputpath%/Output.frag.spv"

@REM GLSL shader variants.
glslc -fshader-stage=vert "%inputpath%/VisibilitySingleSet.vert.glsl" -o "%inputpath%/VisibilitySingleSet.vert.spv"
glslc -fshader-stage=comp "%inputpath%/VisibilityDebugGL.comp.glsl" -o "%inputpath%/VisibilityDebugGL.comp.spv"
glslc -fshader-stage=comp "%inputpath%/MaterialCoordinateGL.comp.glsl" -o "%inputpath%/MaterialCoordinateGL.comp.spv"
	glslc -fshader-stage=comp "%inputpath%/MaterialCountGL.comp.glsl" -o "%inputpath%/MaterialCountGL.comp.spv"