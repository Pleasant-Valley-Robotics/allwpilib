// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstddef>
#include <iterator>
#include <vector>

namespace wpi {

/**
 * This is a simple circular buffer so we don't need to "bucket brigade" copy
 * old values.
 */
template <class T>
class circular_buffer {
 public:
  explicit circular_buffer(size_t size) : m_data(size, T{}) {}

  circular_buffer(const circular_buffer&) = default;
  circular_buffer& operator=(const circular_buffer&) = default;
  circular_buffer(circular_buffer&&) = default;
  circular_buffer& operator=(circular_buffer&&) = default;

  class iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    iterator(circular_buffer* buffer, size_t index)
        : m_buffer{buffer}, m_index{index} {}

    iterator& operator++() {
      ++m_index;
      return *this;
    }
    iterator operator++(int) {
      iterator retval = *this;
      ++(*this);
      return retval;
    }
    bool operator==(const iterator&) const = default;
    reference operator*() { return (*m_buffer)[m_index]; }

   private:
    circular_buffer* m_buffer;
    size_t m_index;
  };

  class const_iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_reference = const T&;

    const_iterator(const circular_buffer* buffer, size_t index)
        : m_buffer{buffer}, m_index{index} {}

    const_iterator& operator++() {
      ++m_index;
      return *this;
    }
    const_iterator operator++(int) {
      const_iterator retval = *this;
      ++(*this);
      return retval;
    }
    bool operator==(const const_iterator&) const = default;
    const_reference operator*() const { return (*m_buffer)[m_index]; }

   private:
    const circular_buffer* m_buffer;
    size_t m_index;
  };

  iterator begin() { return iterator(this, 0); }
  iterator end() { return iterator(this, ::wpi::circular_buffer<T>::size()); }

  const_iterator begin() const { return const_iterator(this, 0); }
  const_iterator end() const {
    return const_iterator(this, ::wpi::circular_buffer<T>::size());
  }

  const_iterator cbegin() const { return const_iterator(this, 0); }
  const_iterator cend() const {
    return const_iterator(this, ::wpi::circular_buffer<T>::size());
  }

  /**
   * Returns number of elements in buffer
   */
  size_t size() const { return m_length; }

  /**
   * Returns value at front of buffer
   */
  T& front() { return (*this)[0]; }

  /**
   * Returns value at front of buffer
   */
  const T& front() const { return (*this)[0]; }

  /**
   * Returns value at back of buffer
   *
   * If there are no elements in the buffer, calling this function results in
   * undefined behavior.
   */
  T& back() { return m_data[(m_front + m_length - 1) % m_data.size()]; }

  /**
   * Returns value at back of buffer
   *
   * If there are no elements in the buffer, calling this function results in
   * undefined behavior.
   */
  const T& back() const {
    return m_data[(m_front + m_length - 1) % m_data.size()];
  }

  /**
   * Push a new value onto the front of the buffer.
   *
   * The value at the back is overwritten if the buffer is full.
   */
  void push_front(T value) {
    if (m_data.size() == 0) {
      return;
    }

    m_front = ModuloDec(m_front);

    m_data[m_front] = value;

    if (m_length < m_data.size()) {
      m_length++;
    }
  }

  /**
   * Push a new value onto the back of the buffer.
   *
   * The value at the front is overwritten if the buffer is full.
   */
  void push_back(T value) {
    if (m_data.size() == 0) {
      return;
    }

    m_data[(m_front + m_length) % m_data.size()] = value;

    if (m_length < m_data.size()) {
      m_length++;
    } else {
      // Increment front if buffer is full to maintain size
      m_front = ModuloInc(m_front);
    }
  }

  /**
   * Push a new value onto the front of the buffer that is constructed with the
   * provided constructor arguments.
   *
   * The value at the back is overwritten if the buffer is full.
   */
  template <class... Args>
  void emplace_front(Args&&... args) {
    if (m_data.size() == 0) {
      return;
    }

    m_front = ModuloDec(m_front);

    m_data[m_front] = T{args...};

    if (m_length < m_data.size()) {
      m_length++;
    }
  }

  /**
   * Push a new value onto the back of the buffer that is constructed with the
   * provided constructor arguments.
   *
   * The value at the front is overwritten if the buffer is full.
   */
  template <class... Args>
  void emplace_back(Args&&... args) {
    if (m_data.size() == 0) {
      return;
    }

    m_data[(m_front + m_length) % m_data.size()] = T{args...};

    if (m_length < m_data.size()) {
      m_length++;
    } else {
      // Increment front if buffer is full to maintain size
      m_front = ModuloInc(m_front);
    }
  }

  /**
   * Pop value at front of buffer.
   *
   * If there are no elements in the buffer, calling this function results in
   * undefined behavior.
   */
  T pop_front() {
    T& temp = m_data[m_front];
    m_front = ModuloInc(m_front);
    m_length--;
    return temp;
  }

  /**
   * Pop value at back of buffer.
   *
   * If there are no elements in the buffer, calling this function results in
   * undefined behavior.
   */
  T pop_back() {
    m_length--;
    return m_data[(m_front + m_length) % m_data.size()];
  }

  /**
   * Resizes internal buffer to given size.
   */
  void resize(size_t size);

  /**
   * Empties internal buffer.
   */
  void reset() {
    m_front = 0;
    m_length = 0;
  }

  /**
   * @return Element at index starting from front of buffer.
   */
  T& operator[](size_t index) {
    return m_data[(m_front + index) % m_data.size()];
  }

  /**
   * @return Element at index starting from front of buffer.
   */
  const T& operator[](size_t index) const {
    return m_data[(m_front + index) % m_data.size()];
  }

 private:
  std::vector<T> m_data;

  // Index of element at front of buffer
  size_t m_front = 0;

  // Number of elements used in buffer
  size_t m_length = 0;

  /**
   * Increment an index modulo the size of the buffer.
   *
   * @param index Index into the buffer.
   * @return The incremented index.
   */
  size_t ModuloInc(size_t index) { return (index + 1) % m_data.size(); }

  /**
   * Decrement an index modulo the size of the buffer.
   *
   * @param index Index into the buffer.
   * @return The decremented index.
   */
  size_t ModuloDec(size_t index) {
    if (index == 0) {
      return m_data.size() - 1;
    } else {
      return index - 1;
    }
  }
};

}  // namespace wpi

#include "wpi/circular_buffer.inc"
