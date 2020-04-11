#include <Windows.h>
#include <iostream>

int main()
{
	DWORD pid;

	HMODULE kernel32 = LoadLibrary("kernel32.dll");

	LPVOID loadLibraryA = GetProcAddress(kernel32, "LoadLibraryA");

	HWND hwnd = FindWindow(0, TEXT("TESTAPP"));

	if (!hwnd) {
		printf("[-] Fail to FindWindow\r\n");
		return 1;
	}

	GetWindowThreadProcessId(hwnd, &pid);

	printf("[+] PID of target : %d\r\n", pid);

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 1, pid);

	if (hProc == INVALID_HANDLE_VALUE) {
		printf("[-] Fail to OpenProcess\r\n");
		return 1;
	}

	// put your path
	const char* path = "D:\\Ecrit\\hook\\test\\main\\Debug\\lib.dll";

	LPVOID pathAddress = VirtualAllocEx(hProc, NULL, strlen(path), (MEM_COMMIT | MEM_RESERVE), 0x40);

	if (!pathAddress)
	{
		printf("[-] Fail to VirtualAllocEx\r\n");
		return 1;
	}

	printf("[+] pathAddress in target : 0x%x\r\n", pathAddress);

	SIZE_T bytesWritten = 0;

	int lpWrite = WriteProcessMemory(hProc, pathAddress, path, strlen(path), &bytesWritten);

	if (!lpWrite)
	{
		printf("[-] Fail to WriteProcessMemory\r\n");
		return 1;
	}

	printf("[+] Bytes written : %d\r\n", bytesWritten);

	HANDLE remoteProc = CreateRemoteThread(hProc, NULL,	0, (LPTHREAD_START_ROUTINE)loadLibraryA, pathAddress, 0, NULL);

	if (!remoteProc)
	{
		printf("[-] Fail to CreateRemoteThread\r\n");
		return 1;
	}

	printf("[+] Injected\r\n");
	
	CloseHandle(hProc);
	
	getchar();

	return 0;
}
