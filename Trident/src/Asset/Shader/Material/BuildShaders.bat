@echo off

set glslcpath=%1
set inputpath=%~dp0

%glslcpath% -fshader-stage=frag "%inputpath%/Lightmapped.frag.glsl" -o "%inputpath%/Lightmapped.frag.spv"
%glslcpath% -fshader-stage=frag "%inputpath%/LightmappedPBR.frag.glsl" -o "%inputpath%/LightmappedPBR.frag.spv"

%glslcpath% -fshader-stage=frag "%inputpath%/PBR.frag.glsl" -o "%inputpath%/PBR.frag.spv"
%glslcpath% -fshader-stage=frag "%inputpath%/PBRDebug.frag.glsl" -o "%inputpath%/PBRDebug.frag.spv"

%glslcpath% -fshader-stage=comp "%inputpath%/Default.glsl" -o "%inputpath%/Default.spv"
%glslcpath% -fshader-stage=comp "%inputpath%/DefaultGL.glsl" -o "%inputpath%/DefaultGL.spv"

%glslcpath% -fshader-stage=comp "%inputpath%/Test.glsl" -o "%inputpath%/Test.spv"
%glslcpath% -fshader-stage=comp "%inputpath%/TestGL.glsl" -o "%inputpath%/TestGL.spv"