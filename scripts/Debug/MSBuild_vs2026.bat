@echo off

call MSBuild Trident.slnx -m -p:Configuration=Debug -t:Build