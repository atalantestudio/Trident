@echo off

call MSBuild Trident.slnx -m -p:Configuration=Debug -p:WarningLevel=4 -t:Build