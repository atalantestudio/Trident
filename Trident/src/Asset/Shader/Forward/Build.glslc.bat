@echo off

set inputpath=%~dp0

glslc -fshader-stage=vert "%inputpath%/Skybox.vert.glsl" -o "%inputpath%/Skybox.vert.spv"
glslc -fshader-stage=frag "%inputpath%/Skybox.frag.glsl" -o "%inputpath%/Skybox.frag.spv"
glslc -fshader-stage=vert "%inputpath%/StaticGeometry.vert.glsl" -o "%inputpath%/StaticGeometry.vert.spv"
glslc -fshader-stage=vert "%inputpath%/DynamicGeometry.vert.glsl" -o "%inputpath%/DynamicGeometry.vert.spv"
glslc -fshader-stage=vert "%inputpath%/DirectionalShadowMap.vert.glsl" -o "%inputpath%/DirectionalShadowMap.vert.spv"
glslc -fshader-stage=vert "%inputpath%/DebugHullWireframe.vert.glsl" -o "%inputpath%/DebugHullWireframe.vert.spv"
glslc -fshader-stage=frag "%inputpath%/DebugHullWireframe.frag.glsl" -o "%inputpath%/DebugHullWireframe.frag.spv"
glslc -fshader-stage=vert "%inputpath%/DebugContactManifold.vert.glsl" -o "%inputpath%/DebugContactManifold.vert.spv"
glslc -fshader-stage=frag "%inputpath%/DebugContactManifold.frag.glsl" -o "%inputpath%/DebugContactManifold.frag.spv"

glslc -fshader-stage=comp "%inputpath%/GI/VertexGeneration.comp.glsl" -o "%inputpath%/GI/VertexGeneration.comp.spv"
glslc -fshader-stage=vert "%inputpath%/GI/Shading.vert.glsl" -o "%inputpath%/GI/Shading.vert.spv"
glslc -fshader-stage=frag "%inputpath%/GI/Shading.frag.glsl" -o "%inputpath%/GI/Shading.frag.spv"