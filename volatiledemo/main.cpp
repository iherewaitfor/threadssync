#include <stdio.h>
#include <windows.h>
 
//全局变量
volatile int signalNum = 0; 
 
DWORD WINAPI threadFuncA(LPVOID lpParamter)
{
	Sleep(2000);
	if (0 == signalNum)
	{
		printf("signalNum not changed!\n");
	}
	else
	{
		printf("signalNum has changed!\n");
	}
 
	return 0;
}
 
DWORD WINAPI threadFuncB(LPVOID lpParamter)
{
	signalNum = 2;
	return 0;
}
 
int main()
{
	HANDLE threadA = CreateThread(NULL, 0, threadFuncA, NULL, 0, NULL);
	HANDLE threadB = CreateThread(NULL, 0, threadFuncB, NULL, 0, NULL);
	
	WaitForSingleObject(threadA, INFINITE);
	CloseHandle(threadA);//CloseHandle只是关闭了系统句柄,该线程还是可以正常的运行
	CloseHandle(threadB);
	return 0;
}