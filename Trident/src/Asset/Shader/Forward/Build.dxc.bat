@echo off

set inputpath=%~dp0

dxc "%inputpath%/Skybox.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/Skybox.vert.dxil"
dxc "%inputpath%/Skybox.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/Skybox.frag.dxil"
dxc "%inputpath%/StaticGeometry.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/StaticGeometry.vert.dxil"
dxc "%inputpath%/DynamicGeometry.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/DynamicGeometry.vert.dxil"
dxc "%inputpath%/DirectionalShadowMap.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/DirectionalShadowMap.vert.dxil"
dxc "%inputpath%/DebugHullWireframe.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/DebugHullWireframe.vert.dxil"
dxc "%inputpath%/DebugHullWireframe.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/DebugHullWireframe.frag.dxil"
dxc "%inputpath%/DebugContactManifold.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/DebugContactManifold.vert.dxil"
dxc "%inputpath%/DebugContactManifold.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/DebugContactManifold.frag.dxil"