@echo off

set inputpath=%~dp0

dxc "%inputpath%/Visibility.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/Visibility.vert.dxil"
dxc "%inputpath%/Visibility.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/Visibility.frag.dxil"
dxc "%inputpath%/VisibilityDebug.comp.hlsl" -T cs_6_0 -Fo "%inputpath%/VisibilityDebug.comp.dxil"
dxc "%inputpath%/MaterialCoordinate.comp.hlsl" -T cs_6_0 -Fo "%inputpath%/MaterialCoordinate.comp.dxil"
dxc "%inputpath%/MaterialCount.comp.hlsl" -T cs_6_0 -Fo "%inputpath%/MaterialCount.comp.dxil"
dxc "%inputpath%/MaterialOffset.comp.hlsl" -T cs_6_0 -Fo "%inputpath%/MaterialOffset.comp.dxil"
dxc "%inputpath%/Output.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/Output.vert.dxil"
dxc "%inputpath%/Output.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/Output.frag.dxil"