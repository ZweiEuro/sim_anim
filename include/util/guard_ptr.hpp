#pragma once
#include <mutex>
// a scoped guard implementation that is movable and protects a m_data set and frees the associated lock on scope end

namespace mg8
{

  template <typename Mutex, typename T>
  class guard_ptr
  {
    std::unique_lock<Mutex> m_guard;
    T *m_data;

  public:
    guard_ptr(Mutex &mutex, T *data) : m_guard(mutex), m_data(data) {}

    // Transfer ownership of the lock and the date. invalidate the data in the original lock
    guard_ptr(guard_ptr &&original) : m_guard(std::move(original.m_guard)), m_data(original.m_data)
    {
      original.m_data = nullptr;
    }

    // transfer ownership as well from the lock and invalidate the data in the original instance
    guard_ptr &operator=(guard_ptr &&original)
    {
      m_guard = std::move(original.m_guard);
      m_data = original.m_data;
      original.m_data = nullptr;
    }

    T &operator*()
    {
      return *m_data;
    }
    T *operator->()
    {
      return m_data;
    }
    T &operator[](size_t i) { return m_data[i]; }

    T *get() { return m_data; }
  };

  template <typename T>
  using mutex_guard_ptr = guard_ptr<std::mutex, T>;

}