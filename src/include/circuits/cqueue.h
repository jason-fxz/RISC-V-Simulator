/**
 * @file cqueue.h
 * @author JasonFan (jasonfanxz@gmail.com)
 * @brief a circuit queue implementation
 * @version 0.1
 * @date 2024-07-28
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef CQUEUE_H
#define CQUEUE_H

#include <cstddef>


namespace jasonfxz {

template <typename Tp, size_t LEN>
class Cqueue {
  private:
    Tp _data[LEN + 1];
    size_t _head = 0;
    size_t _tail = 0;

  public:
    class iterator;
    class const_iterator;

    size_t cap() const { return LEN; }
    size_t size() const { return (_tail - _head + LEN + 1) % (LEN + 1); }
    bool empty() const { return _head == _tail; }
    bool full() const { return (_tail + 1) % (LEN + 1) == _head; }
    bool push(const Tp& value) {
        if (full()) {
            return false;
        }
        _data[_tail] = value;
        _tail = (_tail + 1) % (LEN + 1);
        return true;
    }
    bool pop() {
        if (empty()) {
            return false;
        }
        _head = (_head + 1) % (LEN + 1);
        return true;
    }
    Tp& front() { return _data[_head]; }
    Tp& back() { return _data[(_tail + LEN) % (LEN + 1)]; }
    iterator begin() { return iterator(this, _head); }
    iterator end() { return iterator(this, _tail); }

    class iterator {
      private:
        const Cqueue *_queue;
        size_t _idx;
        iterator(const Cqueue *queue, size_t idx) : _queue(queue), _idx(idx) {}

      public:
        friend Cqueue;
        iterator &operator++() {
            _idx = (_idx + 1) % _queue->cap();
            return *this;
        }
        iterator operator++(int) {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }
        bool operator==(const iterator &rhs) const {
            return _idx == rhs._idx;
        }
        bool operator!=(const iterator &rhs) const {
            return _idx != rhs._idx;
        }
        Tp &operator*() { return _queue->operator[](_idx); }
        Tp *operator->() { return &_queue->operator[](_idx); }
    };

    class const_iterator {
      private:
        const Cqueue *_queue;
        size_t _idx;
        const_iterator(const Cqueue *queue, size_t idx) : _queue(queue), _idx(idx) {}

      public:
        friend Cqueue;
        const_iterator(iterator it) : _queue(it._queue), _idx(it._idx) {}
        const_iterator &operator++() {
            _idx = (_idx + 1) % _queue->cap();
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++*this;
            return tmp;
        }
        bool operator==(const const_iterator &rhs) const {
            return _idx == rhs._idx;
        }
        bool operator!=(const const_iterator &rhs) const {
            return _idx != rhs._idx;
        }
        const Tp &operator*() { return _queue->operator[](_idx); }
        const Tp *operator->() { return &_queue->operator[](_idx); }
    };

};


} // namespace jasonfxz




#endif