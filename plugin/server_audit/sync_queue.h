#include <condition_variable>
#include <mutex>
#include <queue>
#include <future>
#include <chrono>
#include <cstdio> // Include this header for printf

// Thread-safe queue
template<typename T>
class SyncQueue {
public:
  void push(T item) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.size() == max_size_) {
      printf("Queue is full, cannot push item.\n"); // Debugging statement
    } else {
      queue_.push(item);
      printf("Pushed item to queue.\n"); // Debugging statement
      cond_.notify_one();
      printf("Notified condition variable.\n"); // Debugging statement
    }
  }

  T pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    printf("Waiting for condition variable...\n"); // Debugging statement
    cond_.wait(lock,
               [this]() { return !queue_.empty(); });
    printf("Condition variable signaled.\n"); // Debugging statement

    T item = queue_.front();
    queue_.pop();
    printf("Popped item from queue.\n"); // Debugging statement
    return item;
  }

  bool empty() {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
  }

  void resize(size_t new_max_size) {
    std::lock_guard<std::mutex> lock(mutex_);
    printf("Resizing queue to size %zu\n", new_max_size); // Debugging statement
    max_size_ = new_max_size;
  }

  bool is_full() {
    std::lock_guard<std::mutex> lock(mutex_);
    bool is_full = queue_.size() == max_size_;
    printf("Queue is %s\n", is_full ? "full" : "not full"); // Debugging statement
    return is_full;
  }

  SyncQueue(size_t max_size): max_size_(max_size) {
    printf("Initialized SyncQueue with max size %zu\n", max_size); // Debugging statement
  }

private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cond_;
  size_t max_size_;
};