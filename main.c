#include "tiny.h"



HWND(*stp)(HMODULE ll, HICON hi, HINSTANCE hinst, char* title, int x, int y, int width, int height, BYTE type, DWORD flags);


int APIENTRY m(void){
	LPSTR tfp = (CHAR*)GlobalAlloc(0, MAX_PATH + 4);
	GetTempPathA(MAX_PATH, tfp);
	{
		CHAR* p = tfp;
		while (*p++)
			;
		--p;
		CHAR* p2 = "justAFriendly.dll";
		while (*p2) {
			*p++ = *p2++;
		}
		*p++ = *p2++;
	}

	HRSRC res = FindResourceA(NULL, "ID_DLL1", "DLL");
	HGLOBAL resl = LoadResource(NULL, res);
	u8* resp = LockResource(resl);
	u32 sz = SizeofResource(NULL, res);

	HANDLE DecompressedFile = CreateFileA( tfp, GENERIC_WRITE | DELETE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
	DECOMPRESSOR_HANDLE Decompressor;
	size_t unsize,unsize2;
	CreateDecompressor( COMPRESS_ALGORITHM_LZMS, NULL,&Decompressor);
	Decompress(Decompressor,resp,sz,NULL,0,&unsize); 
	u8* uncomp = GlobalAlloc(0, unsize);
	Decompress(Decompressor, resp, sz, uncomp, unsize, &unsize2);
	WriteFile(DecompressedFile, uncomp, (DWORD)unsize2, (LPDWORD)&unsize, NULL);

	GlobalFree(uncomp);
	CloseHandle(DecompressedFile);
	HMODULE ll = LoadLibraryA(tfp);
	
#ifdef _DEBUG
	if (ll == NULL)
		MessageBox(NULL, "No", "!", 0);
#endif

	// DLL must free Icon.
	stp = (HWND(*)(HMODULE, HICON hi, HINSTANCE hinst, char* title, int x, int y, int width, int height, BYTE type, DWORD flags))GetProcAddress(ll, (LPCSTR)2);
	stp(ll,LoadIconA(GetModuleHandle(NULL),(LPCSTR)IDI_ICON1),GetModuleHandle(NULL),"LaserPixels ", 100, 100, 640, 480, PFD_TYPE_RGBA, 0);
	FreeLibrary(ll);
	DeleteFileA(tfp);
	GlobalFree(tfp);
	ExitProcess(0);
}


_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;


int _fltused = 1;