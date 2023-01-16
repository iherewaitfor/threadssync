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
## 参考

《Windows核心编程》
[https://learn.microsoft.com/en-us/windows/win32/sync/synchronization-functions](https://learn.microsoft.com/en-us/windows/win32/sync/synchronization-functions)

[https://blog.csdn.net/lizhenwei0219/article/details/96145663](https://blog.csdn.net/lizhenwei0219/article/details/96145663)

[https://learn.microsoft.com/en-us/windows/win32/sync/using-synchronization](https://learn.microsoft.com/en-us/windows/win32/sync/using-synchronization)