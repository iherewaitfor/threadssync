#include <stdio.h>
#include <windows.h>
 
/*
通过互斥量实现线程间的同步，初始化为没有加锁的状态
*/
HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
 
DWORD WINAPI threadFuncA(LPVOID lpParamter)
{
	//对互斥量加锁,如果已经加锁了则等待其解锁,等待时间为INFINITE(表示永久)
	WaitForSingleObject(mutex, INFINITE);
	/*
		WaitForSingleObject(mutex, INFINITE);
		
		这里可以做对共享资源的操作
		
		ReleaseMutex(mutex);
	*/
	printf("threadFuncA lock mutex，please wait~~~~~~\n");
	Sleep(5000);
	printf("threadFuncA unlock mutex!!!!!!!!!!!!!!!!\n");
	//互斥量解锁
	ReleaseMutex(mutex);
	return 0;
}
 
DWORD WINAPI threadFuncB(LPVOID lpParamter)
{
	Sleep(2000);
	WaitForSingleObject(mutex, INFINITE);
	printf("This is threadFuncB ~~~~~~\n");
	ReleaseMutex(mutex);
	return 0;
}
 
int main()
{
	HANDLE threadA = CreateThread(NULL, 0, threadFuncA, NULL, 0, NULL);
	HANDLE threadB = CreateThread(NULL, 0, threadFuncB, NULL, 0, NULL);
	
	WaitForSingleObject(threadB, INFINITE);
	CloseHandle(threadA);//CloseHandle只是关闭了系统句柄,该线程还是可以正常的运行
	CloseHandle(threadB);
	return 0;
}