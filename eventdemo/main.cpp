#include <stdio.h>
#include <windows.h>
 
//ͨ���¼�����ͨ��
/*�����¼�CreateEvent
LPSECURITY_ATTRIBUTESlpEventAttributes,// ��ȫ����
BOOLbManualReset,// ��λ��ʽ,�����TRUE����ô������ResetEvent��������ԭ�����ź�״̬��FALSE�Զ����¼�״̬��ԭΪ���ź�״̬��
BOOLbInitialState,// ��ʼ״̬,TRUE���ź�,FALESE���ź�
LPCTSTRlpName // ��������
*/
HANDLE threadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
 
DWORD WINAPI threadFuncA(LPVOID lpParamter)
{
	printf("threadFuncA �ȴ��¼����ź�!\n");
	WaitForSingleObject(threadEvent, INFINITE);
	printf("threadFuncA �ȴ��¼��źųɹ�,�����¼��Զ�����Ϊ���ź�״̬!\n");
	return 0;
}
 
DWORD WINAPI threadFuncB(LPVOID lpParamter)
{
	Sleep(5000);
	//���¼������ź�
	SetEvent(threadEvent);
	printf("threadFuncB �����¼��ź�!\n");
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