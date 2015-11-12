@echo off
rem del /F /Q 1.exe
cd %~dp0\
.\g++.exe ..\..\%1 %3 -o ..\..\%2 -static
rem cd ..\..\..\bin\
rem 1.exe