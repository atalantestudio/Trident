@echo off

set glslcpath=%1
set inputpath=%~dp0

%glslcpath% -fshader-stage=vert "%inputpath%/Lightmapper/LightmapAtlas.vert.glsl" -o "%inputpath%/Lightmapper/LightmapAtlas.vert.spv"
%glslcpath% -fshader-stage=frag "%inputpath%/Lightmapper/LightmapAtlas.frag.glsl" -o "%inputpath%/Lightmapper/LightmapAtlas.frag.spv"

%glslcpath% -fshader-stage=comp "%inputpath%/DistanceField.comp.glsl" -o "%inputpath%/DistanceField.comp.spv"
%glslcpath% -fshader-stage=comp "%inputpath%/RasterizeGlyph.comp.glsl" -o "%inputpath%/RasterizeGlyph.comp.spv"