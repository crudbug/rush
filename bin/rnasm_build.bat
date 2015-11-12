@echo off
cd %~dp0
rush.exe ..\src\example\nasm.rs %1 -build