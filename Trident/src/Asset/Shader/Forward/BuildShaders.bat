@echo off

set glslcpath=%1
set inputpath=%~dp0

%glslcpath% -fshader-stage=vert "%inputpath%/Forward.vert.glsl" -o "%inputpath%/Forward.vert.spv"

%glslcpath% -fshader-stage=vert "%inputpath%/DebugHullWireframe.vert.glsl" -o "%inputpath%/DebugHullWireframe.vert.spv"
%glslcpath% -fshader-stage=frag "%inputpath%/DebugHullWireframe.frag.glsl" -o "%inputpath%/DebugHullWireframe.frag.spv"

%glslcpath% -fshader-stage=vert "%inputpath%/DebugContactManifold.vert.glsl" -o "%inputpath%/DebugContactManifold.vert.spv"
%glslcpath% -fshader-stage=frag "%inputpath%/DebugContactManifold.frag.glsl" -o "%inputpath%/DebugContactManifold.frag.spv"

%glslcpath% -fshader-stage=vert "%inputpath%/Static/Static.vert.glsl" -o "%inputpath%/Static/Static.vert.spv"