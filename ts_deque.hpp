#ifndef TS_DEQUE_HPP
#define TS_DEQUE_HPP

#include "ts_alloc.hpp"
#include "ts_iterator.hpp"
#include <cstddef>
namespace TS
{
inline std::size_t deque_buf_size(std::size_t size)
{
    return size < 512 ? 512 / size : 1;
}

template <typename T, typename Ref, typename Ptr>
struct Deque_iterator : public _iterator<random_access_iterator_tag, T>
{
  public:
    using base_iterator = _iterator<random_access_iterator_tag, T>;
    using iterator = Deque_iterator<T, T &, T *>;
    using const_iterator = Deque_iterator<T, const T &, const T *>;
    using self = Deque_iterator<T, Ref, Ptr>;
    using size_type = std::size_t;

    using typename base_iterator::difference_type;
    using typename base_iterator::iterator_category;
    using typename base_iterator::pointer;
    using typename base_iterator::reference;
    using typename base_iterator::value_type;

    using map_pointer = pointer *;

  public:
    static size_type buffer_size()
    {
        return deque_buf_size(sizeof(T));
    }

    Deque_iterator() : _cur(nullptr), _first(nullptr), _last(nullptr), _mapp(nullptr)
    {
    }

    Deque_iterator(T *p, map_pointer mp)
        : _cur(p), _first(*mp), _last(*mp + buffer_size()), _mapp(mp)
    {
    }

    Deque_iterator(const iterator &other)
        : _cur(other._cur), _first(other._first), _last(other._last), _mapp(other._mapp)
    {
    }

    reference operator*() const
    {
        return *_cur;
    }

    pointer operator->() const
    {
        return _cur;
    }

    difference_type operator-(const self &other) const
    {
        return buffer_size() * (_mapp - other._mapp - 1) + (_cur - _first) +
               (other._last - other._cur);
    }

    self &operator++()
    {
        ++_cur;
        if (_cur == _last)
        {
            set_map(_mapp + 1);
            _cur = _first;
        }
        return *this;
    }

    self operator++(int)
    {
        self tmp = *this;
        ++(*this);
        return tmp;
    }

    self &operator--()
    {
        if (_cur == _first)
        {
            set_map(_mapp - 1);
            _cur = _last;
        }
        --_cur;
        return *this;
    }

    self operator--(int)
    {
        self tmp = *this;
        --(*this);
        return tmp;
    }

    self &operator+=(difference_type n)
    {
        difference_type offset = n + (_cur - _first);
        if (offset >= 0 && offset < buffer_size())
        {
            _cur += n;
        }
        else
        {
            difference_type map_offset =
                offset > 0 ? offset / buffer_size() : (offset + 1) / buffer_size() + 1;
            set_map(_mapp + map_offset);
            _cur = _first + (offset - map_offset * buffer_size());
        }
        return *this;
    }

    self operator+(difference_type n) const
    {
        self tmp = *this;
        tmp += n;
        return tmp;
    }

    self &operator-=(difference_type n)
    {
        (*this) += -n;
        return *this;
    }

    self operator-(difference_type n) const
    {
        self tmp = *this;
        tmp -= n;
        return tmp;
    }

    reference operator[](difference_type __n) const
    {
        return *(*this + __n);
    }

    bool operator==(const self &other) const
    {
        return _cur == other._cur;
    }

    void set_map(map_pointer new_map)
    {
        _mapp = new_map;
        _first = *_mapp;
        _last = _first + buffer_size();
    }

  public:
    pointer _cur;
    pointer _first;
    pointer _last;
    map_pointer _mapp;
};
} // namespace TS

#endif