@echo off

call MSBuild Trident.slnx -m -p:Configuration=Release -t:Build