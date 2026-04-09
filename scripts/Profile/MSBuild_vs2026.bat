@echo off

call MSBuild Trident.slnx -m -p:Configuration=Profile -t:Build