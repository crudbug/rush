import rfile.h

void main()
{
	rf.cmd("rnasm_build ..\\src\\rush.cxx")
	rf.cmd("copy /B /Y ..\\src\\rush.exe rush_nasm.exe")
	rf.cmd("rush -nasm ../src/rush.cxx")
	rf.cmd("copy /B /Y ..\\src\\rush.asm rush_nasm.asm")
	rf.cmd("rush_nasm -nasm ../src/rush.cxx")
	if rfile.read_all_n("rush_nasm.asm")==rfile.read_all_n("..\\src\\rush.asm")
		printl "ok"
	else
		printl "error"
}
