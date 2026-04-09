@echo off

call MSBuild Trident.sln -m -p:Configuration=Profile -p:WarningLevel=4 -t:Build