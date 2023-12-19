#pragma once

#include <cstdint>
#include <cstddef>
#include <cstdlib>

template <typename T>
class CircularBuffer {
public:
  CircularBuffer() {}
  ~CircularBuffer() {}

  void Init(size_t n);

  void Push(const T& elt);

  T& Pop();

  size_t PushAvailable();
  size_t PopAvailable();

private:
  T* data_;
  size_t n_;
  unsigned int head_;
  unsigned int tail_;
  volatile int pushed_;
  volatile int popped_;
};


template<typename T>
void CircularBuffer<T>::Init(size_t n) {
  head_ = 0;
  tail_ = 0;
  pushed_ = 0;
  popped_ = 0;
  n_ = n;
  data_ = (unsigned char*)malloc(sizeof(T) * n_);
}


template<typename T>
void CircularBuffer<T>::Push(const T& elt) {
  data_[head_] = elt;
  if (++head_ >= n_) head_ = 0;
  ++pushed_;
}


template<typename T>
T& CircularBuffer<T>::Pop() {
  unsigned int tail = tail_;
  if (++tail_ >= n_) tail_ = 0;
  ++popped_;
  return data_[tail];
}


template<typename T>
size_t CircularBuffer<T>::PushAvailable() {
  return (n_ - 1) - (pushed_ - popped_);
}


template<typename T>
size_t CircularBuffer<T>::PopAvailable() {
  return pushed_ - popped_;
}
