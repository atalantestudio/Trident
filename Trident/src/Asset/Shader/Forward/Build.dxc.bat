@echo off

set dxcpath=%1
set inputpath=%~dp0

%dxcpath% "%inputpath%/Skybox.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/Skybox.vert.dxil"
%dxcpath% "%inputpath%/Skybox.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/Skybox.frag.dxil"
%dxcpath% "%inputpath%/StaticGeometry.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/StaticGeometry.vert.dxil"
%dxcpath% "%inputpath%/DynamicGeometry.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/DynamicGeometry.vert.dxil"
%dxcpath% "%inputpath%/DirectionalShadowMap.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/DirectionalShadowMap.vert.dxil"
%dxcpath% "%inputpath%/DebugHullWireframe.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/DebugHullWireframe.vert.dxil"
%dxcpath% "%inputpath%/DebugHullWireframe.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/DebugHullWireframe.frag.dxil"
%dxcpath% "%inputpath%/DebugContactManifold.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/DebugContactManifold.vert.dxil"
%dxcpath% "%inputpath%/DebugContactManifold.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/DebugContactManifold.frag.dxil"