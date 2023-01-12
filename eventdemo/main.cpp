#include <stdio.h>
#include <windows.h>
 
//通过事件进行通信
/*创建事件CreateEvent
LPSECURITY_ATTRIBUTESlpEventAttributes,// 安全属性
BOOLbManualReset,// 复位方式,如果是TRUE，那么必须用ResetEvent函数来复原到无信号状态。FALSE自动将事件状态复原为无信号状态。
BOOLbInitialState,// 初始状态,TRUE有信号,FALESE无信号
LPCTSTRlpName // 对象名称
*/
HANDLE threadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
 
DWORD WINAPI threadFuncA(LPVOID lpParamter)
{
	printf("threadFuncA 等待事件有信号!\n");
	WaitForSingleObject(threadEvent, INFINITE);
	printf("threadFuncA 等待事件信号成功,并把事件自动设置为无信号状态!\n");
	return 0;
}
 
DWORD WINAPI threadFuncB(LPVOID lpParamter)
{
	Sleep(5000);
	//给事件赋予信号
	SetEvent(threadEvent);
	printf("threadFuncB 给了事件信号!\n");
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