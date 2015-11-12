import rdir.h
import rfile.h

void main()
{
	v=rdir.get_file_bfs('../src/')
	for i=0;i<v.count;i++
		name=rdir.get_name(v[i])
		suffix=rdir.get_suffix(name)
		ifn suffix=='rs'||suffix=='h'||suffix=='cpp'
			continue
		src=rfile.read_all_n(v[i])
		dst=rcode.to_utf8_txt(src)
		if src==dst
			continue
		printl v[i]
		rfile.write_all_n(v[i],dst)
}