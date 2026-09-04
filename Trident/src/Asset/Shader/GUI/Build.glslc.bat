@echo off

set glslcpath=%1
set inputpath=%~dp0

%glslcpath% -fshader-stage=vert "%inputpath%/Subcomponent.vert.glsl" -o "%inputpath%/Subcomponent.vert.spv"
%glslcpath% -fshader-stage=frag "%inputpath%/Subcomponent.frag.glsl" -o "%inputpath%/Subcomponent.frag.spv"
%glslcpath% -fshader-stage=vert "%inputpath%/GlyphSubcomponent.vert.glsl" -o "%inputpath%/GlyphSubcomponent.vert.spv"
%glslcpath% -fshader-stage=frag "%inputpath%/GlyphSubcomponent.frag.glsl" -o "%inputpath%/GlyphSubcomponent.frag.spv"