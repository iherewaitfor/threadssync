#include <stdio.h>
#include <windows.h>


/*
ʹ���ٽ���
*/
CRITICAL_SECTION cs;

DWORD WINAPI threadFuncA(LPVOID lpParamter)
{
	Sleep(2000);
	printf("**************threadFuncA �ȴ��ٽ�������!\n");
	// Request ownership of the critical section.
	EnterCriticalSection(&cs);
	printf("**************threadFuncA �ȴ��ٽ��������ɹ�,�������!\n");
	printf("**************threadFuncA ���ٽ�������!\n");
	// Release ownership of the critical section.
	LeaveCriticalSection(&cs);
	return 0;
}

DWORD WINAPI threadFuncB(LPVOID lpParamter)
{
	EnterCriticalSection(&cs);
	printf("**************threadFuncB ���ٽ�����ס��!\n");
	Sleep(5000);
	printf("**************threadFuncB ���ٽ�������!\n");
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
	CloseHandle(threadA);//CloseHandleֻ�ǹر���ϵͳ���,���̻߳��ǿ�������������
	CloseHandle(threadB);

	// Release resources used by the critical section object.
	DeleteCriticalSection(&cs);

	return 0;
}