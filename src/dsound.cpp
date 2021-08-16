#include <Windows.h>
#include <stdio.h>
#include <direct.h>
#include <io.h>
typedef uintptr_t addr;
#include "MemoryMgr.h"
 
//files check for 1.0e
#include <sys/stat.h>
#include <string>
#include <fstream>
 
#include <TlHelp32.h>
#include "../build/windowed.h"
#include "../build/wideScreen.h"
#include "../build/drs.h"

using namespace std;
 

struct dsound_dll
{
	HMODULE dll;
	FARPROC DirectSoundCreate;
	FARPROC DirectSoundEnumerateA;
	FARPROC DirectSoundEnumerateW;
	FARPROC DllCanUnloadNow;
	FARPROC DllGetClassObject;
	FARPROC DirectSoundCaptureCreate;
	FARPROC DirectSoundCaptureEnumerateA;
	FARPROC DirectSoundCaptureEnumerateW;
	FARPROC GetDeviceID;
	FARPROC DirectSoundFullDuplexCreate;
	FARPROC DirectSoundCreate8;
	FARPROC DirectSoundCaptureCreate8;
} dsound;

WRAPPER void _DirectSoundCreate() { VARJMP(dsound.DirectSoundCreate); }
WRAPPER void _DirectSoundEnumerateA() { VARJMP(dsound.DirectSoundEnumerateA); }
WRAPPER void _DirectSoundEnumerateW() { VARJMP(dsound.DirectSoundEnumerateW); }
WRAPPER void _DllCanUnloadNow() { VARJMP(dsound.DllCanUnloadNow); }
WRAPPER void _DllGetClassObject() { VARJMP(dsound.DllGetClassObject); }
WRAPPER void _DirectSoundCaptureCreate() { VARJMP(dsound.DirectSoundCaptureCreate); }
WRAPPER void _DirectSoundCaptureEnumerateA() { VARJMP(dsound.DirectSoundCaptureEnumerateA); }
WRAPPER void _DirectSoundCaptureEnumerateW() { VARJMP(dsound.DirectSoundCaptureEnumerateW); }
WRAPPER void _GetDeviceID() { VARJMP(dsound.GetDeviceID); }
WRAPPER void _DirectSoundFullDuplexCreate() { VARJMP(dsound.DirectSoundFullDuplexCreate); }
WRAPPER void _DirectSoundCreate8() { VARJMP(dsound.DirectSoundCreate8); }
WRAPPER void _DirectSoundCaptureCreate8() { VARJMP(dsound.DirectSoundCaptureCreate8); }

void init_dsound(HINSTANCE hInst)
{
	TCHAR dsound_dll_path[MAX_PATH];

	GetEnvironmentVariable("windir", dsound_dll_path, MAX_PATH);
	strcat_s(dsound_dll_path, "\\System32\\dsound.dll");

	dsound.dll = LoadLibrary(dsound_dll_path);

	dsound.DirectSoundCreate = GetProcAddress(dsound.dll, "DirectSoundCreate");
	dsound.DirectSoundEnumerateA = GetProcAddress(dsound.dll, "DirectSoundEnumerateA");
	dsound.DirectSoundEnumerateW = GetProcAddress(dsound.dll, "DirectSoundEnumerateW");
	dsound.DllCanUnloadNow = GetProcAddress(dsound.dll, "DllCanUnloadNow");
	dsound.DllGetClassObject = GetProcAddress(dsound.dll, "DllGetClassObject");
	dsound.DirectSoundCaptureCreate = GetProcAddress(dsound.dll, "DirectSoundCaptureCreate");
	dsound.DirectSoundCaptureEnumerateA = GetProcAddress(dsound.dll, "DirectSoundCaptureEnumerateA");
	dsound.DirectSoundCaptureEnumerateW = GetProcAddress(dsound.dll, "DirectSoundCaptureEnumerateW");
	dsound.GetDeviceID = GetProcAddress(dsound.dll, "GetDeviceID");
	dsound.DirectSoundFullDuplexCreate = GetProcAddress(dsound.dll, "DirectSoundFullDuplexCreate");
	dsound.DirectSoundCreate8 = GetProcAddress(dsound.dll, "DirectSoundCreate8");
	dsound.DirectSoundCaptureCreate8 = GetProcAddress(dsound.dll, "DirectSoundCaptureCreate8");
}

int AoK20ab, AoK20, AoC10Ce, AoC10, UserPatch;//(user patch version aofe + 1.5 +1.1 .. etc)

inline bool exists_File(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

void init()
{
	////00699D9C  69 6E 74 65 72 66 61 63 2E 64 72 73              interfac.drs
 //  //00699D9C  77 69 64 65 5F 70 31 2E 64 72 73 20              wide_p1.drs
	//BYTE  SWGBinterfacDrs[13]{ 0x77,0x69,0x64,0x65,0x5F,0x70,0x31,0x2E,0x64,0x72,0x73,0x00,0x00 };
	//writeData((DWORD)0x0699D9C, SWGBinterfacDrs, 13);
	windowedHook();
	//LoadLibraryA("Voobly Mods\\SWBGCC\\Patches\\efPatch\\userpatch.dll");
wideScreenHook();
//windowedHook();
//setDRSLoadHooks(1, true);
//no cd 
Nop(0x050FCFD, 6);
 

}



BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		//load patch here + dsound library
		init_dsound(hInst);
		init();

	}

	if (reason == DLL_PROCESS_DETACH)
	{
		FreeLibrary(dsound.dll);
	}

	return TRUE;
}
