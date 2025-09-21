#ifndef TS_ARRAY_HPP
#define TS_ARRAY_HPP

#include "ts_uninitialized.hpp"
#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace TS
{
template <typename T, std::size_t N> class array;

template <typename T, std::size_t N>
bool operator==(const array<T, N> &lhs, const array<T, N> &rhs);

template <typename T, std::size_t N> class array
{
  public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using iterator = value_type *;
    using const_iterator = const value_type *;

  protected:
    using self = array<T, N>;

  public:
    array() : _data{}
    {
    }

    array(std::initializer_list<T> init)
    {
        if (init.size() > N)
        {
            throw std::out_of_range("Too many elements in initializer list");
        }

        uninitialized_copy_n(init.begin(), init.size(), _data);
        uninitialized_fill_n(_data + init.size(), N - init.size(), T());
    }

    array(const self &other)
    {
        uninitialized_copy_n(other._data, other.size(), _data);
    }

    array(self &&other) noexcept
    {
        for (size_type i = 0; i < N; i++)
        {
            _data[i] = std::move(other._data[i]);
        }
    }

    ~array() = default;

    self &operator=(const self &other)
    {
        if (this != &other)
        {
            uninitialized_copy_n(other._data, other.size(), _data);
        }
        return *this;
    }

    self &operator=(self &&other) noexcept
    {
        if (this != &other)
        {
            for (size_type i = 0; i < N; ++i)
            {
                _data[i] = std::move(other._data[i]);
            }
        }
        return *this;
    }

    reference at(size_type pos)
    {
        return const_cast<reference>(static_cast<const self *>(this)->at(pos));
    }

    const_reference at(size_type pos) const
    {
        if (pos < 0 || pos >= N)
        {
            throw std::out_of_range("out of range");
        }
        return operator[](pos);
    }

    reference operator[](size_type pos)
    {
        return const_cast<reference>(static_cast<const self *>(this)->operator[](pos));
    }

    const_reference operator[](size_type pos) const
    {
        return _data[pos];
    }

    reference front()
    {
        return const_cast<reference>(static_cast<const self *>(this)->front());
    }

    const_reference front() const
    {
        if (empty())
        {
            throw std::logic_error("empty vector");
        }
        return *_data;
    }

    reference back()
    {
        return const_cast<reference>(static_cast<const self *>(this)->back());
    }

    const_reference back() const
    {
        if (empty())
        {
            throw std::logic_error("empty vector");
        }
        return _data[N - 1];
    }

    pointer data() noexcept
    {
        return const_cast<pointer>(static_cast<const self *>(this)->data());
    }

    const_pointer data() const noexcept
    {
        return _data;
    }

    iterator begin() noexcept
    {
        return const_cast<iterator>(static_cast<const self *>(this)->begin());
    }

    const_iterator begin() const noexcept
    {
        return _data;
    }

    const_iterator cbegin() const noexcept
    {
        return _data;
    }

    iterator end() noexcept
    {
        return const_cast<iterator>(static_cast<const self *>(this)->end());
    }

    const_iterator end() const noexcept
    {
        return _data + N;
    }

    const_iterator cend() const noexcept
    {
        return _data + N;
    }

    constexpr bool empty() const
    {
        return N == 0;
    }

    constexpr size_type size() const
    {
        return N;
    }

    constexpr size_type max_size() const
    {
        return N;
    }

    void fill(const T &val)
    {
        uninitialized_fill_n(_data, N, val);
    }

    void swap(self &other) noexcept
    {
        if (this == &other)
        {
            return;
        }
        for (size_type i = 0; i < N; ++i)
        {
            std::swap(_data[i], other._data[i]);
        }
    }

    friend bool operator== <T, N>(const array<T, N> &lhs, const array<T, N> &rhs);

  protected:
    T _data[N];
};

template <typename T, std::size_t N> bool operator==(const array<T, N> &lhs, const array<T, N> &rhs)
{
    if (lhs.size() != rhs.size())
    {
        return false;
    }

    for (std::size_t i = 0; i < N; ++i)
    {
        if (lhs[i] != rhs[i])
        {
            return false;
        }
    }

    return true;
}

template <std::size_t I, typename T, std::size_t N> T &get(array<T, N> &a) noexcept
{
    return const_cast<T &>(get<I>(static_cast<const array<T, N> &>(a)));
}

template <std::size_t I, typename T, std::size_t N> T &&get(array<T, N> &&a) noexcept
{
    static_assert(I >= 0 && I < N);
    return std::move(a[I]);
}

template <std::size_t I, typename T, std::size_t N> const T &get(const array<T, N> &a) noexcept
{
    static_assert(I >= 0 && I < N);
    return a[I];
}

template <std::size_t I, typename T, std::size_t N> const T &&get(const array<T, N> &&a) noexcept
{
    static_assert(I >= 0 && I < N);
    return std::move(a[I]);
}

template <typename T, std::size_t N> void swap(array<T, N> &lhs, array<T, N> &rhs)
{
    lhs.swap(rhs);
}

} // namespace TS

#endif