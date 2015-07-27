#ifndef WORKER_HH_
#define WORKER_HH_

#include "Task.hpp"

enum class state {
  STOP = 0,
  PAUSE,
  START
};

class Worker {
public:
  Worker();
  ~Worker();

  void start(std::condition_variable& cv,
             std::mutex& condvarMutex);

  template<class F, class... Args>
  void start(F&& function, Args&&... args) {
    if (this->running.load() == true)
      throw std::runtime_error("Can't start an already running worker");
    auto task = std::bind(std::forward<F>(function), std::forward<Args>(args)...);
    this->running.store(true);
    this->thread = std::thread(task);
  }
  void stop();
  void stopTask();
  void pauseTask();
  void unpauseTask();
  void waitStopped();
  void setTask(const std::function<void ()>& task);
  void setTask(const Task& task);
  bool isIdle();
  void setReserved(bool status);
  bool isReserved();

private:
  void threadMain(std::condition_variable& cv, std::mutex& condvarMutex);

private:
  Task  task;
  std::thread thread;
  std::mutex  taskMutex;
  std::atomic_bool running;
  std::atomic_bool reserved;
};

#endif /* end of include guard: WORKER_HH_ */
