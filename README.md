# Thread Sync 线程同步
在以下两种基本情况 下，线程之间需要相互通信 
1. 需要让多个线程同时访问一下共享资源，同时不能破坏资源的完整性
2. 一个线程需要通知其他线程某项任务已经完。
3. 


用户态线程同步
- volatile：强制从内存中读取。
- 原子访问：Interlocked系统函数
- 关键段CreticalSection

内核对象线程同步
- 事件Event
- 信号量Semaphore
- 互斥量Mutex


表： 同步机制的性能比较

|线程\微秒|Volatile读取|Volatile写入|Interlocked递增|关键段|SRWLock共享模块|SRWLock独占模式|互斥量|
|:--|:--|:--|:--|:--|:--|:--|:--|
|1|8|8|35|66|66|67|1060|
|2|8|76|153|268|134|148|11082|
|4|9|145|361|768|244|307|23785|

## InterLocked

本例demo代码位于[https://github.com/iherewaitfor/threadssync/tree/main/interlockeddemo](https://github.com/iherewaitfor/threadssync/tree/main/interlockeddemo)。起两个线程对变量g进行自增操作。线程A和线程B都对变量g增加N。预期的结果应该是2N。使用InterLoceked系统函数进行原子操作能得到预期结果。若不使用原子操作，直接 使用g++，则预期结果不是2N，会得到比2N小的数值。

使用了InterlockedAdd或InterlockedIncrement。

```C++
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

```

### InterlockedAdd
[InterlockedAdd](https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedadd)
```C++
LONG InterlockedAdd(
  [in, out] LONG volatile *Addend,
  [in]      LONG          Value
);
```
#### [in, out] Addend

A pointer to the first operand. This value will be replaced with the result of the operation.

#### [in] Value

The second operand.
#### Return value
The function returns the result of the operation.

### InterlockedIncrement
[InterlockedIncrement](https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-interlockedincrement

```C++
LONG InterlockedIncrement(
  [in, out] LONG volatile *Addend
);
```

更多 InterLocked系统函数，可以参考[https://learn.microsoft.com/en-us/windows/win32/sync/synchronization-functions#interlocked-functions](https://learn.microsoft.com/en-us/windows/win32/sync/synchronization-functions#interlocked-functions)
## 关键段Critical Section
本例demo代码位于[https://github.com/iherewaitfor/threadssync/tree/main/criticalsectiondemo](https://github.com/iherewaitfor/threadssync/tree/main/criticalsectiondemo)。起AB两个线程对变量s模拟使用临界区。A线程启动后，立即进入休眠2秒，休眠结束后申请进入临界区。B启动后立即申请进入临界区，由于此时无人占用，B能顺利进入临界区。而A线程申请时，由于 B占用，需要等B释放后，才能继续运行。预期结果，B线程先退出，然后A线程退出。

```C++
#include <stdio.h>
#include <windows.h>


/*
使用临界区
*/
CRITICAL_SECTION cs;

DWORD WINAPI threadFuncA(LPVOID lpParamter)
{
	Sleep(2000);
	printf("**************threadFuncA 等待临界区解锁!\n");
	// Request ownership of the critical section.
	EnterCriticalSection(&cs);
	printf("**************threadFuncA 等待临界区解锁成功,对其加锁!\n");
	printf("**************threadFuncA 把临界区解锁!\n");
	// Release ownership of the critical section.
	LeaveCriticalSection(&cs);
	return 0;
}

DWORD WINAPI threadFuncB(LPVOID lpParamter)
{
	EnterCriticalSection(&cs);
	printf("**************threadFuncB 把临界区锁住了!\n");
	Sleep(5000);
	printf("**************threadFuncB 把临界区解锁!\n");
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
	CloseHandle(threadA);//CloseHandle只是关闭了系统句柄,该线程还是可以正常的运行
	CloseHandle(threadB);

	// Release resources used by the critical section object.
	DeleteCriticalSection(&cs);

	return 0;
}
```
关键段使用核心逻辑:The following example shows how a thread initializes, enters, and releases a critical section. It uses the [InitializeCriticalSectionAndSpinCount](https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-initializecriticalsectionandspincount), [EnterCriticalSection](https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-entercriticalsection), [LeaveCriticalSection](https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-leavecriticalsection), and [DeleteCriticalSection](https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-deletecriticalsection) functions.
```C++
// Global variable
CRITICAL_SECTION CriticalSection; 

int main( void )
{
    ...

    // Initialize the critical section one time only.
    if (!InitializeCriticalSectionAndSpinCount(&CriticalSection, 
        0x00000400) ) 
        return;
    ...

    // Release resources used by the critical section object.
    DeleteCriticalSection(&CriticalSection);
}

DWORD WINAPI ThreadProc( LPVOID lpParameter )
{
    ...

    // Request ownership of the critical section.
    EnterCriticalSection(&CriticalSection); 

    // Access the shared resource.

    // Release ownership of the critical section.
    LeaveCriticalSection(&CriticalSection);

    ...
return 1;
}
```

## 事件同步Event
本例demo代码位于[https://github.com/iherewaitfor/threadssync/tree/main/eventdemo](https://github.com/iherewaitfor/threadssync/tree/main/eventdemo)。起AB两个线程，模板B线程执行完一些操作后，A线程再执行。。A线程启动后立即通过[WaitForSingleObject](https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject)等待事件。B启动后休眠5秒，然后他用[SetEvent](https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-setevent)触发事件并退出。A收到事件后，继续运行退出。

```C++
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

```
### CreateEvent
[CreateEventA](https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-createeventa)

```C++
HANDLE CreateEventA(
  [in, optional] LPSECURITY_ATTRIBUTES lpEventAttributes,
  [in]           BOOL                  bManualReset,
  [in]           BOOL                  bInitialState,
  [in, optional] LPCSTR                lpName
);
```

#### [in, optional] LPSECURITY_ATTRIBUTES lpEventAttributes,
A pointer to a SECURITY_ATTRIBUTES structure. If this parameter is NULL, the handle cannot be inherited by child processes.

####  [in]           BOOL                  bManualReset
If this parameter is TRUE, the function creates a manual-reset event object, which requires the use of the ResetEvent function to set the event state to nonsignaled. If this parameter is FALSE, the function creates an auto-reset event object, and system automatically resets the event state to nonsignaled after a single waiting thread has been released.
####   [in]           BOOL                  bInitialState
If this parameter is TRUE, the initial state of the event object is signaled; otherwise, it is nonsignaled.

####   [in, optional] LPCSTR                lpName
The name of the event object. The name is limited to MAX_PATH characters. Name comparison is case sensitive.

## 参考

《Windows核心编程》
[https://learn.microsoft.com/en-us/windows/win32/sync/synchronization-functions](https://learn.microsoft.com/en-us/windows/win32/sync/synchronization-functions)

[https://blog.csdn.net/lizhenwei0219/article/details/96145663](https://blog.csdn.net/lizhenwei0219/article/details/96145663)

[https://learn.microsoft.com/en-us/windows/win32/sync/using-synchronization](https://learn.microsoft.com/en-us/windows/win32/sync/using-synchronization)

关键段Critical Section[https://learn.microsoft.com/en-us/windows/win32/sync/using-critical-section-objects](https://learn.microsoft.com/en-us/windows/win32/sync/using-critical-section-objects)

事件Event[https://learn.microsoft.com/en-us/windows/win32/sync/using-event-objects](https://learn.microsoft.com/en-us/windows/win32/sync/using-event-objects)