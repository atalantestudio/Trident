@echo off

set glslcpath=%1
set inputpath=%~dp0

%glslcpath% -fshader-stage=vert "%inputpath%/Lightmapper/Rasterization.vert.glsl" -o "%inputpath%/Lightmapper/Rasterization.vert.spv"
%glslcpath% -fshader-stage=frag "%inputpath%/Lightmapper/Rasterization.frag.glsl" -o "%inputpath%/Lightmapper/Rasterization.frag.spv"

%glslcpath% -fshader-stage=comp "%inputpath%/Lightmapper/TraceRays.comp.glsl" -o "%inputpath%/Lightmapper/TraceRays.comp.spv"
%glslcpath% -fshader-stage=comp "%inputpath%/Lightmapper/IntermediateDilation.comp.glsl" -o "%inputpath%/Lightmapper/IntermediateDilation.comp.spv"
%glslcpath% -fshader-stage=comp "%inputpath%/Lightmapper/HorizontalBilateralFilter.comp.glsl" -o "%inputpath%/Lightmapper/HorizontalBilateralFilter.comp.spv"
%glslcpath% -fshader-stage=comp "%inputpath%/Lightmapper/VerticalBilateralFilter.comp.glsl" -o "%inputpath%/Lightmapper/VerticalBilateralFilter.comp.spv"

%glslcpath% -fshader-stage=vert "%inputpath%/Lightmapper/Dilation.vert.glsl" -o "%inputpath%/Lightmapper/Dilation.vert.spv"
%glslcpath% -fshader-stage=frag "%inputpath%/Lightmapper/Dilation.frag.glsl" -o "%inputpath%/Lightmapper/Dilation.frag.spv"

%glslcpath% -fshader-stage=comp "%inputpath%/DistanceField.comp.glsl" -o "%inputpath%/DistanceField.comp.spv"
%glslcpath% -fshader-stage=comp "%inputpath%/RasterizeGlyph.comp.glsl" -o "%inputpath%/RasterizeGlyph.comp.spv"