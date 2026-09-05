@echo off

set inputpath=%~dp0

dxc "%inputpath%/Subcomponent.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/Subcomponent.vert.dxil"
dxc "%inputpath%/Subcomponent.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/Subcomponent.frag.dxil"
dxc "%inputpath%/GlyphSubcomponent.vert.hlsl" -T vs_6_0 -Fo "%inputpath%/GlyphSubcomponent.vert.dxil"
dxc "%inputpath%/GlyphSubcomponent.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/GlyphSubcomponent.frag.dxil"