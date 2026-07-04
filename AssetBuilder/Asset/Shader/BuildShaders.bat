@echo off

set glslcpath=%1
set inputpath=%~dp0

%glslcpath% -fshader-stage=vert "%inputpath%/Lightmapper/RasterizeLightmaps.vert.glsl" -o "%inputpath%/Lightmapper/RasterizeLightmaps.vert.spv"
%glslcpath% -fshader-stage=frag "%inputpath%/Lightmapper/RasterizeLightmaps.frag.glsl" -o "%inputpath%/Lightmapper/RasterizeLightmaps.frag.spv"

%glslcpath% -fshader-stage=comp "%inputpath%/Lightmapper/TraceRays.comp.glsl" -o "%inputpath%/Lightmapper/TraceRays.comp.spv"

%glslcpath% -fshader-stage=comp "%inputpath%/DistanceField.comp.glsl" -o "%inputpath%/DistanceField.comp.spv"
%glslcpath% -fshader-stage=comp "%inputpath%/RasterizeGlyph.comp.glsl" -o "%inputpath%/RasterizeGlyph.comp.spv"