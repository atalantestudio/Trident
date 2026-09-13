@echo off

set inputpath=%~dp0

glslc -fshader-stage=frag "%inputpath%/DDGI.frag.glsl" -o "%inputpath%/DDGI.frag.spv"
glslc -fshader-stage=frag "%inputpath%/Lightmapped.frag.glsl" -o "%inputpath%/Lightmapped.frag.spv"
glslc -fshader-stage=frag "%inputpath%/LightmappedPBR.frag.glsl" -o "%inputpath%/LightmappedPBR.frag.spv"
glslc -fshader-stage=frag "%inputpath%/PBR.frag.glsl" -o "%inputpath%/PBR.frag.spv"
glslc -fshader-stage=frag "%inputpath%/PBRDebug.frag.glsl" -o "%inputpath%/PBRDebug.frag.spv"
glslc -fshader-stage=comp "%inputpath%/Default.glsl" -o "%inputpath%/Default.spv"
glslc -fshader-stage=comp "%inputpath%/Test.glsl" -o "%inputpath%/Test.spv"

@REM GLSL shader variants.
glslc -fshader-stage=comp "%inputpath%/DefaultGL.glsl" -o "%inputpath%/DefaultGL.spv"
glslc -fshader-stage=comp "%inputpath%/TestGL.glsl" -o "%inputpath%/TestGL.spv"