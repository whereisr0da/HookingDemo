#include <Windows.h>
#include <iostream>

void function() {
	printf("normal working state on : 0x%x\r\n", *function);
}

void main() {

	SetConsoleTitle(TEXT("TESTAPP"));

	printf("some program\r\n");

	while (true) {
		Sleep(1000);
		function();
	}

	getchar();
}