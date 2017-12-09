#include "utils\TexFile.h"
#include <cstdio>
#include <Windows.h>
int main(int argc, char* argv[])
{
	TexFile t;
	t.LoadFromFile("tst.tpf");
	if (t.error())
		return 0;
	CreateDirectoryA("output", NULL);
	char buffer[0x100] = "output\\";
	char* fn = buffer + 7;
	for (unsigned i = 0; i < t.count(); ++i) {
		TexFile::Texture& tex = t[i];
		strcpy(fn, tex.filename);
		FILE* fh = fopen(buffer, "wb");
		fwrite(tex.base, sizeof(unsigned char), tex.size, fh);
		fclose(fh);
	}
	return 0;
}