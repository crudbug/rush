
del /F /Q ..\bin\*.ilk
del /F /Q ..\bin\*.pdb

del /F /Q .\*.ncb
attrib -R -S -A -H .\*.suo
del /F /Q .\*.suo
del /F /Q .\*.user

del /F /Q .\proj\*.vsp
rem del /F /Q .\proj\*.filters
del /F /Q .\proj\*.psess
del /F /Q .\proj\*.sdf
del /F /Q .\proj\*.ncb
attrib -R -S -A -H .\proj\*.suo
del /F /Q .\proj\*.suo
del /F /Q .\proj\*.user

rd /S /Q .\proj\debug
rd /S /Q .\proj\Release

del /F /Q .\rush.asm
del /F /Q .\rush.cpp
del /F /Q .\rush.exe

del /F /Q .\example\test\*.exe
del /F /Q .\example\test\*.cpp
del /F /Q ..\bin\rush_gpp.asm
del /F /Q ..\bin\rush_nasm.asm
del /F /Q ..\bin\rush_gpp.exe
del /F /Q ..\bin\rush_nasm.exe

pause