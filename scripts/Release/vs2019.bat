@echo off

call MSBuild Trident.sln -m -p:Configuration=Release -t:Build