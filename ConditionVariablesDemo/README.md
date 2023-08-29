# 条件变量ConditionVariablesDemo 
典型应用：生产者消费者

只能同一进程内使用。不可跨进程。

需要依赖关键段（或SRW） 配合使用。

- 初始化
  - InitializeConditionVariable
- 生产条件
  - WakeConditionVariable
    - 若此时有线程阻塞在SleepConditionVariableCS，会被唤醒。
  - 如通知：缓冲区有空间啦
- 使用
  - 通过关键段获取到互斥资源
  - 等待条件SleepConditionVariableCS
    - 如缓存区是否有空间（非满）
    - 调用该函数，若条件不满足，会被阻塞（即之前没有被调用过WakeConditionVariable）
    - 阻塞时，会释放对应的关键段
    - 返回时，会重新获取到对应关键段
      - 记得处理完逻辑，要释放对应的关键段
      - LeaveCriticalSection (&BufferLock);
  - 可以生产对应的条件，唤醒等待该条件的线程
    - 如之前逻辑是写入缓冲区。处理完逻辑后，表示缓冲区非空了。
    - 可以叫醒等待该条件的消费线程。
      - WakeConditionVariable (&BufferNotEmpty)
    


# 相关函数
- InitializeConditionVariable
- SleepConditionVariableCS
- SleepConditionVariableSRW
- WakeAllConditionVariable
- WakeConditionVariable

# 参考

[https://learn.microsoft.com/en-us/windows/win32/sync/condition-variables](https://learn.microsoft.com/en-us/windows/win32/sync/condition-variables)

[https://learn.microsoft.com/en-us/windows/win32/sync/using-condition-variables](https://learn.microsoft.com/en-us/windows/win32/sync/using-condition-variables)