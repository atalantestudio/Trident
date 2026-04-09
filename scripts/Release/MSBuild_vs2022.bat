@echo off

call MSBuild Trident.sln -m -p:Configuration=Release -p:WarningLevel=4 -t:Build