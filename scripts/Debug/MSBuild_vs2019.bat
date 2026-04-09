@echo off

call MSBuild Trident.sln -m -p:Configuration=Debug -t:Build