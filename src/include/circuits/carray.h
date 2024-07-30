/**
 * @file carray.h
 * @author JasonFan (jasonfanxz@gmail.com)
 * @brief a circuit array implementation
 * @version 0.1
 * @date 2024-07-28
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef CARRAY_H
#define CARRAY_H

#include <cstddef>
#include <utility>

namespace jasonfxz {


template <typename Tp, size_t LEN>
class Carray {
  private:
    std::pair<bool, Tp> data[LEN];

  public:
    class iterator;
    class const_iterator;

    int size() const { return LEN; }
    bool busy(size_t index) const { return data[index].first; }
    Tp &operator[](size_t index) { return data[index].second; }
    bool full() const {
        for (size_t i = 0; i < LEN; i++) {
            if (!data[i].first) {
                return false;
            }
        }
        return true;
    }
    int getpos() const {
        for (size_t i = 0; i < LEN; i++) {
            if (!data[i].first) {
                return i;
            }
        }
        return -1;
    }
    void set(size_t index, const Tp &value) {
        data[index].first = true;
        data[index].second = value;
    }
    void clean(size_t index) { data[index].first = false; }
    bool insert(const Tp &value) {
        for (size_t i = 0; i < LEN; i++) {
            if (!data[i].first) {
                data[i].first = true;
                data[i].second = value;
                return true;
            }
        }
        return false;
    }
    iterator begin() { return iterator(this, 0); }
    iterator end() { return iterator(this, LEN); }

    class iterator {
      private:
        Carray *carray;
        size_t index;
        iterator(Carray *carray, size_t index) : carray(carray), index(index) {}
      public:
        friend Carray;

        iterator &operator++() {
            index++;
            return *this;
        }
        iterator operator++(int) {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }
        bool operator==(const iterator &rhs) const { return index == rhs.index; }
        bool operator!=(const iterator &rhs) const { return index != rhs.index; }
        Tp &operator*() { return carray->data[index].second; }
        Tp *operator->() { return &carray->data[index].second; }
    };

    class const_iterator {
      private:
        const Carray *carray;
        size_t index;

        const_iterator(const Carray *carray, size_t index) : carray(carray), index(index) {}
      public:
        friend Carray;
        const_iterator(iterator it) : carray(it.carray), index(it.index) {}
        const_iterator &operator++() {
            index++;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++*this;
            return tmp;
        }
        bool operator==(const const_iterator &rhs) const { return index == rhs.index; }
        bool operator!=(const const_iterator &rhs) const { return index != rhs.index; }
        const Tp &operator*() { return carray->data[index].second; }
        const Tp *operator->() { return &carray->data[index].second; }
    };

};


} // namespace jasonfxz

#endif