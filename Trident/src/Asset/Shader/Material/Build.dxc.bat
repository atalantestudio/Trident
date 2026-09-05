@echo off

set inputpath=%~dp0

dxc "%inputpath%/Lightmapped.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/Lightmapped.frag.dxil"
dxc "%inputpath%/LightmappedPBR.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/LightmappedPBR.frag.dxil"
dxc "%inputpath%/PBR.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/PBR.frag.dxil"

@REM TODO(forward/dx12): Add PBRDebug HLSL shader.
@REM dxc "%inputpath%/PBRDebug.frag.hlsl" -T ps_6_0 -Fo "%inputpath%/PBRDebug.frag.dxil"

dxc "%inputpath%/Default.hlsl" -T cs_6_0 -Fo "%inputpath%/Default.dxil"
dxc "%inputpath%/Test.hlsl" -T cs_6_0 -Fo "%inputpath%/Test.dxil"