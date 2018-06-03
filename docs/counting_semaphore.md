### Counting Semaphore
This class will allow N notifies before allowing the wait(s) to pass.  This is useful where a single consumer is waiting for N producers to complete.

## basic_semaphore<Mutex, ConditionVariable>
A semaphore that does not allow sharing/copying.

## basic_shared_semaphore<Mutex, ConditionVariable>
A semaphore that can be shared
