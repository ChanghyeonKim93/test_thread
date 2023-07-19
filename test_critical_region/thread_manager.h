#ifndef _THREAD_MANAGER_H_
#define _THREAD_MANAGER_H_
#include <iostream>
#include <future>

class ThreadManager
{
public:
  ThreadManager()
  {
    terminate_signal_ = terminate_promise_.get_future();
  }
  ~ThreadManager()
  {
    terminate_promise_.set_value();
    std::cout << "Thread manager is deleted.\n";
  }

  std::shared_future<void> GetTerminateSignal()
  {
    return terminate_signal_;
  }

private:
  std::shared_future<void> terminate_signal_;
  std::promise<void> terminate_promise_;
};

#endif