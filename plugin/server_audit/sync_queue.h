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
      fprintf(stderr, "Queue is full, cannot push item. THIS IS SHOULD HAVE HAPPENED \n"); // Debugging statement
    } else {
      queue_.push(item);
      fprintf(stderr, "Pushed item to queue.\n"); // Debugging statement
      cond_.notify_one();
      fprintf(stderr, "Notified condition variable.\n"); // Debugging statement
    }
  }

  T pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    fprintf(stderr, "Waiting for condition variable...\n"); // Debugging statement
    cond_.wait(lock,
               [this]() { return !queue_.empty(); });
    fprintf(stderr, "Condition variable signaled.\n"); // Debugging statement

    T item = queue_.front();
    queue_.pop();
    fprintf(stderr, "Popped item from queue.\n"); // Debugging statement
    return item;
  }

  bool empty() {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
  }

  void resize(size_t new_max_size) {
    std::lock_guard<std::mutex> lock(mutex_);
    fprintf(stderr, "Resizing queue to size %zu\n", new_max_size); // Debugging statement
    max_size_ = new_max_size;
  }

  bool is_full() {
    std::lock_guard<std::mutex> lock(mutex_);
    bool is_full = queue_.size() == max_size_;
    fprintf(stderr, "Queue is %s\n", is_full ? "full" : "not full"); // Debugging statement
    return is_full;
  }

  std::vector<T> flush() {
      std::lock_guard<std::mutex> lock(mutex_);
      std::vector<T> all_messages;
      all_messages.reserve(queue_.size());
      while (!queue_.empty()) {
          all_messages.push_back(queue_.front());
          queue_.pop();
      }
      return all_messages;
  }

  SyncQueue(size_t max_size): max_size_(max_size) {
    fprintf(stderr, "Initialized SyncQueue with max size %zu\n", max_size); // Debugging statement
  }

private:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cond_;
  size_t max_size_;
};