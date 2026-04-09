@echo off

call MSBuild Trident.sln -m -p:Configuration=Profile -t:Build