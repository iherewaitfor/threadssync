#include <stdio.h>
#include <windows.h>
 
/*
使用信号量
参数2:当前可用的信号的量个数 范围为(0 - 参数3)
参数3:信号量的最大值
*/
HANDLE signalSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
 
DWORD WINAPI threadFuncA(LPVOID lpParamter)
{
	Sleep(2000);
	WaitForSingleObject(signalSemaphore, INFINITE);
	printf("threadFuncA 使用了一个信号量!\n");
	Sleep(2000);
	printf("threadFuncA 释放了一个信号量!\n");
	ReleaseSemaphore(signalSemaphore, 1, NULL);
	return 0;
}
 
DWORD WINAPI threadFuncB(LPVOID lpParamter)
{
	//这里会使信号量减1
	WaitForSingleObject(signalSemaphore, INFINITE);
	printf("threadFuncB 使用了一个信号量!\n");
	Sleep(5000);
	//信号量+1
	printf("threadFuncB 释放了一个信号量!\n");
	ReleaseSemaphore(signalSemaphore, 1, NULL);
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