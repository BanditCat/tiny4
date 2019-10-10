#include "tiny.h"



HWND(*stp)(HMODULE, HICON hi, HINSTANCE hinst, char* title, int x, int y, int width, int height, BYTE type, DWORD flags);


int APIENTRY
m(void)
{
	HRSRC res = FindResourceA(NULL, "ID_DLL1", "DLL");
	HGLOBAL resl = LoadResource(NULL, res);
	u8* resp = LockResource(resl);
	u32 sz = SizeofResource(NULL, res);

	HANDLE DecompressedFile = CreateFileA( "friendly.dll", GENERIC_WRITE | DELETE, 0,	NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,	NULL);
	DECOMPRESSOR_HANDLE Decompressor;
	size_t unsize,unsize2;
	CreateDecompressor( COMPRESS_ALGORITHM_LZMS, NULL,&Decompressor);
	Decompress(Decompressor,resp,sz,NULL,0,&unsize); 
	u8* uncomp = GlobalAlloc(0, unsize);
	Decompress(Decompressor, resp, sz, uncomp, unsize, &unsize2);
	WriteFile(DecompressedFile, uncomp, (DWORD)unsize2, (LPDWORD)&unsize, NULL);

	CloseHandle(DecompressedFile);
	GlobalFree(uncomp);

	HMODULE ll = LoadLibraryA("friendly");
	// DLL must free library and Icon.
	stp = (HWND(*)(HMODULE ll, HICON hi, HINSTANCE hinst, char* title, int x, int y, int width, int height, BYTE type, DWORD flags))GetProcAddress(ll, (LPCSTR)2);

	stp(ll,LoadIconA(GetModuleHandle(NULL),(LPCSTR)IDI_ICON1),GetModuleHandle(NULL),"LaserPixels ", 100, 100, 640, 480, PFD_TYPE_RGBA, 0);
	
	return 42;
}


int _fltused = 1;