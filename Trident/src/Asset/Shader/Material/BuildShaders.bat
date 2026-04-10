@echo off

set glslcpath=%1
set inputpath=%~dp0

%glslcpath% -fshader-stage=frag "%inputpath%/PBR.frag.glsl" -o "%inputpath%/PBR.frag.spv"
%glslcpath% -fshader-stage=frag "%inputpath%/PBRSingleSet.frag.glsl" -o "%inputpath%/PBRSingleSet.frag.spv"

%glslcpath% -fshader-stage=frag "%inputpath%/PBRSeparatedAORoughness.frag.glsl" -o "%inputpath%/PBRSeparatedAORoughness.frag.spv"
%glslcpath% -fshader-stage=frag "%inputpath%/PBRSeparatedAORoughnessSingleSet.frag.glsl" -o "%inputpath%/PBRSeparatedAORoughnessSingleSet.frag.spv"

%glslcpath% -fshader-stage=comp "%inputpath%/Default.glsl" -o "%inputpath%/Default.spv"
%glslcpath% -fshader-stage=comp "%inputpath%/DefaultGL.glsl" -o "%inputpath%/DefaultGL.spv"

%glslcpath% -fshader-stage=comp "%inputpath%/Test.glsl" -o "%inputpath%/Test.spv"
%glslcpath% -fshader-stage=comp "%inputpath%/TestGL.glsl" -o "%inputpath%/TestGL.spv"