#ifndef __MEMORYMGR
#define __MEMORYMGR

#define WRAPPER __declspec(naked)
#define DEPRECATED __declspec(deprecated)
#define EAXJMP(a) { _asm mov eax, a _asm jmp eax }
#define VARJMP(a) { _asm jmp a }
#define WRAPARG(a) UNREFERENCED_PARAMETER(a)

#define NOVMT __declspec(novtable)
#define SETVMT(a) *((DWORD_PTR*)this) = (DWORD_PTR)a
#include <Windows.h>
enum
{
	PATCH_CALL,
	PATCH_JUMP,
	PATCH_NOTHING,
};

#define PTRFROMCALL(uintptr_t) (uint32_t)(*(uint32_t*)((uint32_t)uintptr_t+1) + (uint32_t)uintptr_t + 5)
#define INTERCEPT(saved, func, a) \
do { \
	saved = PTRFROMCALL(a); \
	InjectHook(a, func); \
} while (0)

template<typename T, typename AT> inline void
Patch(AT uintptr_tess, T value)
{
	DWORD		dwProtect[2];
	VirtualProtect((void*)uintptr_tess, sizeof(T), PAGE_EXECUTE_READWRITE, &dwProtect[0]);
	*(T*)uintptr_tess = value;
	VirtualProtect((void*)uintptr_tess, sizeof(T), dwProtect[0], &dwProtect[1]);
}

inline void
PatchBytes(DWORD uintptr_tess, void* value, size_t nCount)
{
	DWORD		dwProtect[2];
	VirtualProtect((void*)uintptr_tess, nCount, PAGE_EXECUTE_READWRITE, &dwProtect[0]);
	memcpy((void*)uintptr_tess, value, nCount);
	VirtualProtect((void*)uintptr_tess, nCount, dwProtect[0], &dwProtect[1]);
}

template<typename AT> inline void
Nop(AT uintptr_tess, unsigned int nCount)
{
	DWORD		dwProtect[2];
	VirtualProtect((void*)uintptr_tess, nCount, PAGE_EXECUTE_READWRITE, &dwProtect[0]);
	memset((void*)uintptr_tess, 0x90, nCount);
	VirtualProtect((void*)uintptr_tess, nCount, dwProtect[0], &dwProtect[1]);
}

template<typename AT, typename HT> inline void
InjectHook(AT uintptr_tess, HT hook, unsigned int nType = PATCH_NOTHING)
{
	DWORD		dwProtect[2];
	switch (nType)
	{
	case PATCH_JUMP:
		VirtualProtect((void*)uintptr_tess, 5, PAGE_EXECUTE_READWRITE, &dwProtect[0]);
		*(BYTE*)uintptr_tess = 0xE9;
		break;
	case PATCH_CALL:
		VirtualProtect((void*)uintptr_tess, 5, PAGE_EXECUTE_READWRITE, &dwProtect[0]);
		*(BYTE*)uintptr_tess = 0xE8;
		break;
	default:
		VirtualProtect((void*)((DWORD)uintptr_tess + 1), 4, PAGE_EXECUTE_READWRITE, &dwProtect[0]);
		break;
	}
	DWORD		dwHook;
	_asm
	{
		mov		eax, hook
		mov		dwHook, eax
	}

	*(ptrdiff_t*)((DWORD)uintptr_tess + 1) = (DWORD)dwHook - (DWORD)uintptr_tess - 5;
	if (nType == PATCH_NOTHING)
		VirtualProtect((void*)((DWORD)uintptr_tess + 1), 4, dwProtect[0], &dwProtect[1]);
	else
		VirtualProtect((void*)uintptr_tess, 5, dwProtect[0], &dwProtect[1]);
}

inline void ExtractCall(void* dst, uintptr_t a)
{
	*(uintptr_t*)dst = (uintptr_t)(*(uintptr_t*)(a + 1) + a + 5);
}
template<typename T>
inline void InterceptCall(void* dst, T func, uintptr_t a)
{
	ExtractCall(dst, a);
	InjectHook(a, func);
}
template<typename T>
inline void InterceptVmethod(void* dst, T func, uintptr_t a)
{
	*(uintptr_t*)dst = *(uintptr_t*)a;
	Patch(a, func);
}
inline void __cdecl setHookJE(void* addr, void* newAddr)
{
	unsigned long c;
	unsigned char j = 0x0F;
	unsigned char jg = 0x84;
	WriteProcessMemory(GetCurrentProcess(), addr, &j, 1, (SIZE_T*)&c);
	WriteProcessMemory(GetCurrentProcess(), (char*)addr + 1, &jg, 1, (SIZE_T*)&c);
	char* r = (char*)newAddr - (char*)addr + (char*)-5 - 1;
	WriteProcessMemory(GetCurrentProcess(), (char*)addr + 2, &r, 4, (SIZE_T*)&c);

}
inline void __cdecl setHookJA(void* addr, void* newAddr)
{
	unsigned long c;
	unsigned char j = 0x0F;
	unsigned char jg = 0x87;
	WriteProcessMemory(GetCurrentProcess(), addr, &j, 1, (SIZE_T*)&c);
	WriteProcessMemory(GetCurrentProcess(), (char*)addr + 1, &jg, 1, (SIZE_T*)&c);
	char* r = (char*)newAddr - (char*)addr + (char*)-5 - 1;
	WriteProcessMemory(GetCurrentProcess(), (char*)addr + 2, &r, 4, (SIZE_T*)&c);

}
inline void __cdecl setHookCall(void* addr, void* newAddr)
{
	unsigned long c;
	unsigned char j = 0xE8;
	WriteProcessMemory(GetCurrentProcess(), addr, &j, 1, (SIZE_T*)&c);
	char* r = (char*)newAddr - (char*)addr + (char*)-5;
	WriteProcessMemory(GetCurrentProcess(), (char*)addr + 1, &r, 4, (SIZE_T*)&c);

}
inline void __cdecl writeByte(DWORD addr, BYTE val)
{
	WriteProcessMemory(GetCurrentProcess(), (void*)addr, &val, 1, 0);
}
inline void __cdecl writeData(DWORD addr, void* data, int len)
{
	WriteProcessMemory(GetCurrentProcess(), (void*)addr, data, len, 0);
}

inline void __cdecl writeDwordF(DWORD addr, DWORD val)
{
	WriteProcessMemory(GetCurrentProcess(), (void*)(addr + 0x400000 + 1), &val, 4, 0);
}
inline void __cdecl writeDwordF3(DWORD addr, DWORD val)
{
	WriteProcessMemory(GetCurrentProcess(), (void*)(addr + 0x400000), &val, 3, 0);

}
inline void __cdecl writeDwordF1(DWORD addr, DWORD val)
{
	WriteProcessMemory(GetCurrentProcess(), (void*)(addr + 0x400000), &val, 3, 0);
}
inline void __cdecl writeDword(DWORD addr, DWORD val)
{
	WriteProcessMemory(GetCurrentProcess(), (void*)addr, &val, 4, 0);
}
#endif