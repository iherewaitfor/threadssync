#include <stdio.h>
#include <windows.h>
 
/*
ͨ��������ʵ���̼߳��ͬ������ʼ��Ϊû�м�����״̬
*/
HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
 
DWORD WINAPI threadFuncA(LPVOID lpParamter)
{
	//�Ի���������,����Ѿ���������ȴ������,�ȴ�ʱ��ΪINFINITE(��ʾ����)
	WaitForSingleObject(mutex, INFINITE);
	/*
		WaitForSingleObject(mutex, INFINITE);
		
		����������Թ�����Դ�Ĳ���
		
		ReleaseMutex(mutex);
	*/
	printf("threadFuncA lock mutex��please wait~~~~~~\n");
	Sleep(5000);
	printf("threadFuncA unlock mutex!!!!!!!!!!!!!!!!\n");
	//����������
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
	CloseHandle(threadA);//CloseHandleֻ�ǹر���ϵͳ���,���̻߳��ǿ�������������
	CloseHandle(threadB);
	return 0;
}