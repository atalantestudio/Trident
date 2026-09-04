@echo off

set dxcpath=%1
set inputpath=%~dp0

%dxcpath% "%inputpath%/Lightmapper/GBuffer.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/Lightmapper/GBuffer.vert.dxil"
%dxcpath% "%inputpath%/Lightmapper/GBuffer.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/Lightmapper/GBuffer.frag.dxil"
%dxcpath% "%inputpath%/Lightmapper/Lightmap.rgen.hlsl" -T lib_6_3 -Fo "%inputpath%/Lightmapper/Lightmap.rgen.dxil"
%dxcpath% "%inputpath%/Lightmapper/Lightmap.rchit.hlsl" -T lib_6_3 -Fo "%inputpath%/Lightmapper/Lightmap.rchit.dxil"
%dxcpath% "%inputpath%/Lightmapper/Lightmap.rmiss.hlsl" -T lib_6_3 -Fo "%inputpath%/Lightmapper/Lightmap.rmiss.dxil"
%dxcpath% "%inputpath%/Lightmapper/IntermediateDilation.comp.hlsl" -T cs_6_0 -Fo "%inputpath%/Lightmapper/IntermediateDilation.comp.dxil"
%dxcpath% "%inputpath%/Lightmapper/HorizontalBilateralFilter.comp.hlsl" -T cs_6_0 -Fo "%inputpath%/Lightmapper/HorizontalBilateralFilter.comp.dxil"
%dxcpath% "%inputpath%/Lightmapper/VerticalBilateralFilter.comp.hlsl" -T cs_6_0 -Fo "%inputpath%/Lightmapper/VerticalBilateralFilter.comp.dxil"
%dxcpath% "%inputpath%/Lightmapper/Dilation.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/Lightmapper/Dilation.vert.dxil"
%dxcpath% "%inputpath%/Lightmapper/Dilation.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/Lightmapper/Dilation.frag.dxil"
%dxcpath% "%inputpath%/DistanceField.comp.hlsl" -T cs_6_0 -Fo "%inputpath%/DistanceField.comp.dxil"
%dxcpath% "%inputpath%/RasterizeGlyph.comp.hlsl" -T cs_6_0 -Fo "%inputpath%/RasterizeGlyph.comp.dxil"