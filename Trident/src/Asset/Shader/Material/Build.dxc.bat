@echo off

set dxcpath=%1
set inputpath=%~dp0

%dxcpath% "%inputpath%/Lightmapped.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/Lightmapped.frag.dxil"
%dxcpath% "%inputpath%/LightmappedPBR.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/LightmappedPBR.frag.dxil"
%dxcpath% "%inputpath%/PBR.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/PBR.frag.dxil"

@REM TODO(forward/dx12): Add PBRDebug HLSL shader.
@REM %dxcpath% "%inputpath%/PBRDebug.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/PBRDebug.frag.dxil"

%dxcpath% "%inputpath%/Default.hlsl" -T cs_6_0 -Fo "%inputpath%/Default.dxil"
%dxcpath% "%inputpath%/Test.hlsl" -T cs_6_0 -Fo "%inputpath%/Test.dxil"