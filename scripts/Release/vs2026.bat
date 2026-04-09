@echo off

call MSBuild Trident.slnx -m -p:Configuration=Release -p:WarningLevel=4 -t:Build