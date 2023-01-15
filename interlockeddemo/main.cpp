#include <stdio.h>
#include <windows.h>
 

long g = 0;
const int forTime = 1000000;
DWORD WINAPI threadFuncA(LPVOID lpParamter)
{
	printf("threadFuncA  start \n");
	for(int i = 0; i < forTime; i ++){
		//g++; //�����԰�ȫ��˫�߳�������Ԥ�ڽ��
		InterlockedAdd(&g, 1); //ԭ�Ӳ�������֤������ȷ
		//InterlockedIncrement(&g);//ԭ�Ӳ�������֤������ȷ
	}
	printf("threadFuncA end  %d\n", g);

	return 0;
}
 
DWORD WINAPI threadFuncB(LPVOID lpParamter)
{
	printf("threadFuncB  start \n");
	for(int i = 0; i < forTime; i ++){
		//g++; //�����԰�ȫ��˫�߳�������Ԥ�ڽ��
		InterlockedAdd(&g, 1); //ԭ�Ӳ�������֤������ȷ
		//InterlockedIncrement(&g);//ԭ�Ӳ�������֤������ȷ
	}
	printf("threadFuncB end %d\n", g);
	Sleep(1000);//��֤B����˳�
	return 0;
}
 
int main()
{
	HANDLE threadA = CreateThread(NULL, 0, threadFuncA, NULL, 0, NULL);
	HANDLE threadB = CreateThread(NULL, 0, threadFuncB, NULL, 0, NULL);
	
	WaitForSingleObject(threadB, INFINITE);
	printf("main: the expected number is  %d . the result is %s \n", forTime * 2, forTime*2 == g ? "success": "failed");
	CloseHandle(threadA);//CloseHandleֻ�ǹر���ϵͳ���,���̻߳��ǿ�������������
	CloseHandle(threadB);
	return 0;
}