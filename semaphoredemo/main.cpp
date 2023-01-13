#include <stdio.h>
#include <windows.h>
 
/*
ʹ���ź���
����2:��ǰ���õ��źŵ������� ��ΧΪ(0 - ����3)
����3:�ź��������ֵ
*/
HANDLE signalSemaphore = CreateSemaphore(NULL, 1, 1, NULL);
 
DWORD WINAPI threadFuncA(LPVOID lpParamter)
{
	Sleep(2000);
	WaitForSingleObject(signalSemaphore, INFINITE);
	printf("threadFuncA ʹ����һ���ź���!\n");
	Sleep(2000);
	printf("threadFuncA �ͷ���һ���ź���!\n");
	ReleaseSemaphore(signalSemaphore, 1, NULL);
	return 0;
}
 
DWORD WINAPI threadFuncB(LPVOID lpParamter)
{
	//�����ʹ�ź�����1
	WaitForSingleObject(signalSemaphore, INFINITE);
	printf("threadFuncB ʹ����һ���ź���!\n");
	Sleep(5000);
	//�ź���+1
	printf("threadFuncB �ͷ���һ���ź���!\n");
	ReleaseSemaphore(signalSemaphore, 1, NULL);
	return 0;
}
 
int main()
{
	HANDLE threadA = CreateThread(NULL, 0, threadFuncA, NULL, 0, NULL);
	HANDLE threadB = CreateThread(NULL, 0, threadFuncB, NULL, 0, NULL);
	
	WaitForSingleObject(threadA, INFINITE);
	CloseHandle(threadA);//CloseHandleֻ�ǹر���ϵͳ���,���̻߳��ǿ�������������
	CloseHandle(threadB);
 
	return 0;
}