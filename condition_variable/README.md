# condition_variable使用
标准的生产者消费者使用场景

涉及的资源
- 线程
  - std::thread
    - join
  - 生产者线程
  - 消费者线程
- 互斥资源
  - 产品池
  - 使用std::mutext进行互斥
    - std::unique_lock
    - 使用了包装器std::unique_lock，进行自动上锁和解锁。
- 条件变量（2个）
  - std::condition_variable
  - 可以消费啦（通知消费者）
    - std::condition_variable CAR_NOT_ZERO
  - 可以生产啦（通知生产者）
    - std::condition_variable CAR_NOT_MAX
  