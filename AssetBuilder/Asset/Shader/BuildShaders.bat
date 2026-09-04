@echo off

set glslcpath=%1
set inputpath=%~dp0

%glslcpath% -fshader-stage=vert "%inputpath%/Lightmapper/GBuffer.vert.glsl" -o "%inputpath%/Lightmapper/GBuffer.vert.spv"
%glslcpath% -fshader-stage=frag "%inputpath%/Lightmapper/GBuffer.frag.glsl" -o "%inputpath%/Lightmapper/GBuffer.frag.spv"

%glslcpath% -fshader-stage=rgen --target-spv=spv1.4 "%inputpath%/Lightmapper/Lightmap.rgen.glsl" -o "%inputpath%/Lightmapper/Lightmap.rgen.spv"
%glslcpath% -fshader-stage=rchit --target-spv=spv1.4 "%inputpath%/Lightmapper/Lightmap.rchit.glsl" -o "%inputpath%/Lightmapper/Lightmap.rchit.spv"
%glslcpath% -fshader-stage=rmiss --target-spv=spv1.4 "%inputpath%/Lightmapper/Lightmap.rmiss.glsl" -o "%inputpath%/Lightmapper/Lightmap.rmiss.spv"

@REM TODO(asset-builder/lightmapper): Remove when EXT_hardware_ray_tracing is ready.
%glslcpath% -fshader-stage=comp "%inputpath%/Lightmapper/TraceRays.comp.glsl" -o "%inputpath%/Lightmapper/TraceRays.comp.spv"

%glslcpath% -fshader-stage=comp "%inputpath%/Lightmapper/IntermediateDilation.comp.glsl" -o "%inputpath%/Lightmapper/IntermediateDilation.comp.spv"
%glslcpath% -fshader-stage=comp "%inputpath%/Lightmapper/HorizontalBilateralFilter.comp.glsl" -o "%inputpath%/Lightmapper/HorizontalBilateralFilter.comp.spv"
%glslcpath% -fshader-stage=comp "%inputpath%/Lightmapper/VerticalBilateralFilter.comp.glsl" -o "%inputpath%/Lightmapper/VerticalBilateralFilter.comp.spv"

%glslcpath% -fshader-stage=vert "%inputpath%/Lightmapper/Dilation.vert.glsl" -o "%inputpath%/Lightmapper/Dilation.vert.spv"
%glslcpath% -fshader-stage=frag "%inputpath%/Lightmapper/Dilation.frag.glsl" -o "%inputpath%/Lightmapper/Dilation.frag.spv"

%glslcpath% -fshader-stage=comp "%inputpath%/DistanceField.comp.glsl" -o "%inputpath%/DistanceField.comp.spv"
%glslcpath% -fshader-stage=comp "%inputpath%/RasterizeGlyph.comp.glsl" -o "%inputpath%/RasterizeGlyph.comp.spv"

dxc "%inputpath%/Lightmapper/Lightmap.rgen.hlsl" -T lib_6_3 -Fo "%inputpath%/Lightmapper/Lightmap.rgen.dxil"
dxc "%inputpath%/Lightmapper/Lightmap.rchit.hlsl" -T lib_6_3 -Fo "%inputpath%/Lightmapper/Lightmap.rchit.dxil"
dxc "%inputpath%/Lightmapper/Lightmap.rmiss.hlsl" -T lib_6_3 -Fo "%inputpath%/Lightmapper/Lightmap.rmiss.dxil"