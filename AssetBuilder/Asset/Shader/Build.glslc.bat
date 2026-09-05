@echo off

set inputpath=%~dp0

glslc -fshader-stage=vert "%inputpath%/Lightmapper/GBuffer.vert.glsl" -o "%inputpath%/Lightmapper/GBuffer.vert.spv"
glslc -fshader-stage=frag "%inputpath%/Lightmapper/GBuffer.frag.glsl" -o "%inputpath%/Lightmapper/GBuffer.frag.spv"
glslc -fshader-stage=rgen --target-spv=spv1.4 "%inputpath%/Lightmapper/Lightmap.rgen.glsl" -o "%inputpath%/Lightmapper/Lightmap.rgen.spv"
glslc -fshader-stage=rchit --target-spv=spv1.4 "%inputpath%/Lightmapper/Lightmap.rchit.glsl" -o "%inputpath%/Lightmapper/Lightmap.rchit.spv"
glslc -fshader-stage=rmiss --target-spv=spv1.4 "%inputpath%/Lightmapper/Lightmap.rmiss.glsl" -o "%inputpath%/Lightmapper/Lightmap.rmiss.spv"
glslc -fshader-stage=comp "%inputpath%/Lightmapper/IntermediateDilation.comp.glsl" -o "%inputpath%/Lightmapper/IntermediateDilation.comp.spv"
glslc -fshader-stage=comp "%inputpath%/Lightmapper/HorizontalBilateralFilter.comp.glsl" -o "%inputpath%/Lightmapper/HorizontalBilateralFilter.comp.spv"
glslc -fshader-stage=comp "%inputpath%/Lightmapper/VerticalBilateralFilter.comp.glsl" -o "%inputpath%/Lightmapper/VerticalBilateralFilter.comp.spv"
glslc -fshader-stage=vert "%inputpath%/Lightmapper/Dilation.vert.glsl" -o "%inputpath%/Lightmapper/Dilation.vert.spv"
glslc -fshader-stage=frag "%inputpath%/Lightmapper/Dilation.frag.glsl" -o "%inputpath%/Lightmapper/Dilation.frag.spv"
glslc -fshader-stage=comp "%inputpath%/DistanceField.comp.glsl" -o "%inputpath%/DistanceField.comp.spv"
glslc -fshader-stage=comp "%inputpath%/RasterizeGlyph.comp.glsl" -o "%inputpath%/RasterizeGlyph.comp.spv"