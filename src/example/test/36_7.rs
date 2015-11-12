/*
read self
*/

import "rfile.h"

void main()
{
	rfile file("../src/example/test/36_7.rs")
	file.read_all.sub(3).print
	//auto close file in destructor
}
