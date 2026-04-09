@echo off

call MSBuild Trident.sln -m -p:Configuration=Debug -p:WarningLevel=4 -t:Build