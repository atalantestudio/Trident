@echo off

set glslcpath=%1
set inputpath=%~dp0

%glslcpath% -fshader-stage=vert "%inputpath%/Visibility.vert.glsl" -o "%inputpath%/Visibility.vert.spv"
%glslcpath% -fshader-stage=vert "%inputpath%/VisibilitySingleSet.vert.glsl" -o "%inputpath%/VisibilitySingleSet.vert.spv"
%glslcpath% -fshader-stage=frag "%inputpath%/Visibility.frag.glsl" -o "%inputpath%/Visibility.frag.spv"

%glslcpath% -fshader-stage=comp "%inputpath%/VisibilityDebug.comp.glsl" -o "%inputpath%/VisibilityDebug.comp.spv"
%glslcpath% -fshader-stage=comp "%inputpath%/VisibilityDebugGL.comp.glsl" -o "%inputpath%/VisibilityDebugGL.comp.spv"

%glslcpath% -fshader-stage=comp "%inputpath%/MaterialCoordinate.comp.glsl" -o "%inputpath%/MaterialCoordinate.comp.spv"
%glslcpath% -fshader-stage=comp "%inputpath%/MaterialCoordinateGL.comp.glsl" -o "%inputpath%/MaterialCoordinateGL.comp.spv"

%glslcpath% -fshader-stage=comp "%inputpath%/MaterialCount.comp.glsl" -o "%inputpath%/MaterialCount.comp.spv"
%glslcpath% -fshader-stage=comp "%inputpath%/MaterialCountGL.comp.glsl" -o "%inputpath%/MaterialCountGL.comp.spv"

%glslcpath% -fshader-stage=comp "%inputpath%/MaterialOffset.comp.glsl" -o "%inputpath%/MaterialOffset.comp.spv"

%glslcpath% -fshader-stage=vert "%inputpath%/Output.vert.glsl" -o "%inputpath%/Output.vert.spv"
%glslcpath% -fshader-stage=frag "%inputpath%/Output.frag.glsl" -o "%inputpath%/Output.frag.spv"