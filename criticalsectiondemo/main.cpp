#include <stdio.h>
#include <windows.h>


/*
使用临界区
*/
CRITICAL_SECTION cs;

DWORD WINAPI threadFuncA(LPVOID lpParamter)
{
	Sleep(2000);
	printf("**************threadFuncA 等待临界区解锁!\n");
	// Request ownership of the critical section.
	EnterCriticalSection(&cs);
	printf("**************threadFuncA 等待临界区解锁成功,对其加锁!\n");
	printf("**************threadFuncA 把临界区解锁!\n");
	// Release ownership of the critical section.
	LeaveCriticalSection(&cs);
	return 0;
}

DWORD WINAPI threadFuncB(LPVOID lpParamter)
{
	EnterCriticalSection(&cs);
	printf("**************threadFuncB 把临界区锁住了!\n");
	Sleep(5000);
	printf("**************threadFuncB 把临界区解锁!\n");
	// Release ownership of the critical section.
	LeaveCriticalSection(&cs);
	return 0;
}

int main()
{
	// Initialize the critical section one time only.
	if (!InitializeCriticalSectionAndSpinCount(&cs, 0x00000400)) {
		return 0;
	}
	HANDLE threadA = CreateThread(NULL, 0, threadFuncA, NULL, 0, NULL);
	HANDLE threadB = CreateThread(NULL, 0, threadFuncB, NULL, 0, NULL);

	WaitForSingleObject(threadA, INFINITE);
	CloseHandle(threadA);//CloseHandle只是关闭了系统句柄,该线程还是可以正常的运行
	CloseHandle(threadB);

	// Release resources used by the critical section object.
	DeleteCriticalSection(&cs);

	return 0;
}