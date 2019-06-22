#include <Windows.h>
#include <iostream>

DWORD functionAddress = 0x411770;

void hook() {

	printf("Hooked\r\n");
}

void writeTheHook() {

	printf("[+] hookAddress : 0x%x\r\n", *hook);

	const int instructionSize = 5;

	DWORD relativeAddress = ((DWORD)hook - (DWORD)functionAddress) - instructionSize;

	printf("[+] relativeAddress : 0x%x\r\n", relativeAddress);

	DWORD defaultProtection;
	BOOL pageChange = VirtualProtect((LPVOID)functionAddress, instructionSize, PAGE_EXECUTE_READWRITE, &defaultProtection);

	if (!pageChange)
	{
		printf("[-] Fail to VirtualProtect\r\n");
		return;
	}

	byte jmpInstruction[instructionSize];

	jmpInstruction[0] = 0xE9; // jmp
	jmpInstruction[1] = relativeAddress;
	jmpInstruction[2] = (relativeAddress >> 8);
	jmpInstruction[3] = (relativeAddress >> 16);
	jmpInstruction[4] = (relativeAddress >> 24);

	SIZE_T bytesWritten;

	int lpWrite = WriteProcessMemory(GetCurrentProcess(), (LPVOID)functionAddress, jmpInstruction, instructionSize, &bytesWritten);

	if (lpWrite == 0)
	{
		printf("[-] Fail to WriteProcessMemory\r\n");
		return;
	}

	printf("[+] Jump writed at 0x%x\r\n", functionAddress);

	DWORD oldProtect;
	pageChange = VirtualProtect((LPVOID)functionAddress, instructionSize, defaultProtection, &oldProtect);

	if (!pageChange)
	{
		printf("[-] Fail to VirtualProtect\r\n");
		return;
	}
}

DWORD WINAPI thread(LPVOID param) {

	printf("[+] Thread started\r\n");

	writeTheHook();

	while (true) {
		Sleep(100);
	}

	FreeLibraryAndExitThread((HMODULE)param, 0);

	return 0;
}

bool WINAPI DllMain(HINSTANCE hinstDLL,	DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason)
	{

	case DLL_PROCESS_ATTACH:

		CreateThread(0, 0, thread, hinstDLL, 0, 0);

		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;

	}

	return true;
}