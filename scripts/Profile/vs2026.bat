@echo off

call MSBuild Trident.slnx -m -p:Configuration=Profile -p:WarningLevel=4 -t:Build