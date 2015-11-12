@echo off
cd %~dp0
rush.exe ..\src\example\gpp.rs %1 -O0 -build