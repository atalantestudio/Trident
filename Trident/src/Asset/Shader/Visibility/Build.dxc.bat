@echo off

set dxcpath=%1
set inputpath=%~dp0

%dxcpath% "%inputpath%/Visibility.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/Visibility.vert.dxil"
%dxcpath% "%inputpath%/Visibility.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/Visibility.frag.dxil"
%dxcpath% "%inputpath%/VisibilityDebug.comp.hlsl" -T cs_6_0 -Fo "%inputpath%/VisibilityDebug.comp.dxil"
%dxcpath% "%inputpath%/MaterialCoordinate.comp.hlsl" -T cs_6_0 -Fo "%inputpath%/MaterialCoordinate.comp.dxil"
%dxcpath% "%inputpath%/MaterialCount.comp.hlsl" -T cs_6_0 -Fo "%inputpath%/MaterialCount.comp.dxil"
%dxcpath% "%inputpath%/MaterialOffset.comp.hlsl" -T cs_6_0 -Fo "%inputpath%/MaterialOffset.comp.dxil"
%dxcpath% "%inputpath%/Output.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/Output.vert.dxil"
%dxcpath% "%inputpath%/Output.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/Output.frag.dxil"