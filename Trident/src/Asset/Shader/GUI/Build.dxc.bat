@echo off

set dxcpath=%1
set inputpath=%~dp0

%dxcpath% "%inputpath%/Subcomponent.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/Subcomponent.vert.dxil"
%dxcpath% "%inputpath%/Subcomponent.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/Subcomponent.frag.dxil"
%dxcpath% "%inputpath%/GlyphSubcomponent.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/GlyphSubcomponent.vert.dxil"
%dxcpath% "%inputpath%/GlyphSubcomponent.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/GlyphSubcomponent.frag.dxil"