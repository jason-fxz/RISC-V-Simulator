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

#include <utility>

namespace jasonfxz {


template <typename Tp, int LEN>
class Carray {
  private:
    std::pair<bool, Tp> data[LEN];
    int _count;

  public:
    class iterator;
    class const_iterator;
    Carray() {
        clear();
    }
    int size() const { return LEN; }
    int count() const { return _count; }
    bool busy(int index) const { return data[index].first; }
    Tp &operator[](int index) { return data[index].second; }
    bool full() const {
        for (int i = 0; i < LEN; i++) {
            if (!data[i].first) {
                return false;
            }
        }
        return true;
    }
    // int getpos() const {
    //     for (int i = 0; i < LEN; i++) {
    //         if (!data[i].first) {
    //             return i;
    //         }
    //     }
    //     return -1;
    // }
    // void set(int index, const Tp &value) {
    //     data[index].first = true;
    //     data[index].second = value;
    // }
    void clean(int index) { data[index].first = false; --_count;}

    void remove(int index) {
        for (int i = index; i + 1 < _count; ++i) {
            data[i] = data[i + 1];
        }
        data[_count - 1].first = false;
        --_count;

    }
    bool insert(const Tp &value) {
        for (int i = 0; i < LEN; i++) {
            if (!data[i].first) {
                data[i].first = true;
                data[i].second = value;
                ++_count;
                return true;
            }
        }
        return false;
    }
    void clear() {
        for (int i = 0; i < LEN; i++) {
            data[i].first = false;
        }
        _count = 0;
    }
    iterator begin() { return iterator(this, 0); }
    iterator end() { return iterator(this, LEN); }

    class iterator {
      private:
        Carray *carray;
        int index;
        iterator(Carray *carray, int index) : carray(carray), index(index) {}
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
        bool ok() { return carray->data[index].first; }
        std::pair<bool, Tp> &operator*() { return carray->data[index]; }
        std::pair<bool, Tp> *operator->() { return &carray->data[index]; }
    };

    class const_iterator {
      private:
        const Carray *carray;
        int index;

        const_iterator(const Carray *carray, int index) : carray(carray), index(index) {}
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
        bool ok() { return carray->data[index].first; }
        const std::pair<bool, Tp> &operator*() { return carray->data[index]; }
        const std::pair<bool, Tp> *operator->() { return &carray->data[index]; }
    };

};


} // namespace jasonfxz

#endif