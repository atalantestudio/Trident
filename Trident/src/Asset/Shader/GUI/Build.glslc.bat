@echo off

set inputpath=%~dp0

glslc -fshader-stage=vert "%inputpath%/Subcomponent.vert.glsl" -o "%inputpath%/Subcomponent.vert.spv"
glslc -fshader-stage=frag "%inputpath%/Subcomponent.frag.glsl" -o "%inputpath%/Subcomponent.frag.spv"
glslc -fshader-stage=vert "%inputpath%/GlyphSubcomponent.vert.glsl" -o "%inputpath%/GlyphSubcomponent.vert.spv"
glslc -fshader-stage=frag "%inputpath%/GlyphSubcomponent.frag.glsl" -o "%inputpath%/GlyphSubcomponent.frag.spv"