#include <stdio.h>
#include <windows.h>
 

long g = 0;
const int forTime = 1000000;
DWORD WINAPI threadFuncA(LPVOID lpParamter)
{
	printf("threadFuncA  start \n");
	for(int i = 0; i < forTime; i ++){
		//g++; //非线性安全，双线程自增非预期结果
		InterlockedAdd(&g, 1); //原子操作，保证自增正确
		//InterlockedIncrement(&g);//原子操作，保证自增正确
	}
	printf("threadFuncA end  %d\n", g);

	return 0;
}
 
DWORD WINAPI threadFuncB(LPVOID lpParamter)
{
	printf("threadFuncB  start \n");
	for(int i = 0; i < forTime; i ++){
		//g++; //非线性安全，双线程自增非预期结果
		InterlockedAdd(&g, 1); //原子操作，保证自增正确
		//InterlockedIncrement(&g);//原子操作，保证自增正确
	}
	printf("threadFuncB end %d\n", g);
	Sleep(1000);//保证B最后退出
	return 0;
}
 
int main()
{
	HANDLE threadA = CreateThread(NULL, 0, threadFuncA, NULL, 0, NULL);
	HANDLE threadB = CreateThread(NULL, 0, threadFuncB, NULL, 0, NULL);
	
	WaitForSingleObject(threadB, INFINITE);
	printf("main: the expected number is  %d . the result is %s \n", forTime * 2, forTime*2 == g ? "success": "failed");
	CloseHandle(threadA);//CloseHandle只是关闭了系统句柄,该线程还是可以正常的运行
	CloseHandle(threadB);
	return 0;
}