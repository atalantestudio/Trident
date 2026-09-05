@echo off

set inputpath=%~dp0

dxc "%inputpath%/Lightmapper/GBuffer.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/Lightmapper/GBuffer.vert.dxil"
dxc "%inputpath%/Lightmapper/GBuffer.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/Lightmapper/GBuffer.frag.dxil"
dxc "%inputpath%/Lightmapper/Lightmap.rgen.hlsl" -T lib_6_3 -Fo "%inputpath%/Lightmapper/Lightmap.rgen.dxil"
dxc "%inputpath%/Lightmapper/Lightmap.rchit.hlsl" -T lib_6_3 -Fo "%inputpath%/Lightmapper/Lightmap.rchit.dxil"
dxc "%inputpath%/Lightmapper/Lightmap.rmiss.hlsl" -T lib_6_3 -Fo "%inputpath%/Lightmapper/Lightmap.rmiss.dxil"
dxc "%inputpath%/Lightmapper/IntermediateDilation.comp.hlsl" -T cs_6_0 -Fo "%inputpath%/Lightmapper/IntermediateDilation.comp.dxil"
dxc "%inputpath%/Lightmapper/HorizontalBilateralFilter.comp.hlsl" -T cs_6_0 -Fo "%inputpath%/Lightmapper/HorizontalBilateralFilter.comp.dxil"
dxc "%inputpath%/Lightmapper/VerticalBilateralFilter.comp.hlsl" -T cs_6_0 -Fo "%inputpath%/Lightmapper/VerticalBilateralFilter.comp.dxil"
dxc "%inputpath%/Lightmapper/Dilation.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/Lightmapper/Dilation.vert.dxil"
dxc "%inputpath%/Lightmapper/Dilation.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/Lightmapper/Dilation.frag.dxil"
dxc "%inputpath%/DistanceField.comp.hlsl" -T cs_6_0 -Fo "%inputpath%/DistanceField.comp.dxil"
dxc "%inputpath%/RasterizeGlyph.comp.hlsl" -T cs_6_0 -Fo "%inputpath%/RasterizeGlyph.comp.dxil"