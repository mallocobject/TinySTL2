#ifndef TS_VECTOR_HPP
#define TS_VECTOR_HPP

#include "ts_alloc.hpp"
#include "ts_uninitialized.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace TS
{
const std::size_t VECTOR_INIT_SIZE = 10;

template <typename T, typename Alloc = malloc_alloc> class vector
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
    using data_allocator = simple_alloc<T, Alloc>;
    using self = vector<T, Alloc>;

  public:
    ~vector()
    {
        if (_start)
        {
            zero_capacity();
        }
    }

    vector() : _start(nullptr), _finish(nullptr), _end_of_storage(nullptr)
    {
        initialize(VECTOR_INIT_SIZE);
    }

    vector(size_type count) : vector()
    {
        fill_initialize(count, T());
    }

    vector(size_type count, const T &val) : vector()
    {
        fill_initialize(count, val);
    }

    // template <typename InputIter> vector(InputIter first, InputIter last) : vector()
    // {
    //     size_type count = (size_type)(last - first);
    //     initialize(count);
    //     for (iterator cur = _start; first != last; ++first, ++cur)
    //     {
    //         construct(&*cur, *first);
    //     }
    // }

    vector(const self &other) : vector()
    {
        size_type count = other.size();
        initialize(count);
        // iterator first = other._start;
        // for (iterator cur = _start; cur != _finish; ++cur, ++first)
        // {
        //     construct(&*cur, *first);
        // }
        uninitialized_copy(other._start, other._finish, _start);
        _finish = _start + count;
    }

    vector(self &&other) noexcept : _start(other._start), _finish(other._finish), _end_of_storage(other._end_of_storage)
    {
        other._start = nullptr;
        other._finish = nullptr;
        other._end_of_storage = nullptr;
    }

    vector(std::initializer_list<T> init) : vector()
    {
        size_type count = init.size();
        initialize(count);
        uninitialized_copy(init.begin(), init.end(), _start);
        _finish = _start + count;
    }

    vector &operator=(const self &other)
    {
        if (&other == this)
        {
            return *this;
        }

        size_type count = other.size();
        iterator first = other._start;
        iterator last = other._finish;

        if (capacity() > count)
        {
            clear(_start, _start + count);
        }
        else
        {
            zero_capacity();
            reserve(count);
        }
        _finish = uninitialized_copy(first, last, _start);

        return *this;
    }

    vector &operator=(self &&other) noexcept
    {
        if (&other == this)
        {
            return *this;
        }

        _start = other._start;
        _finish = other._finish;
        _end_of_storage = other._end_of_storage;

        other._start = nullptr;
        other._finish = nullptr;
        other._end_of_storage = nullptr;

        return *this;
    }

    vector &assign(const self &other)
    {
        operator=(other);
        return *this;
    }

    // element access

    reference at(difference_type n)
    {
        return const_cast<reference>(static_cast<const self *>(this)->at(n));
    }

    const_reference at(difference_type n) const
    {
        if (n >= size() || n < 0)
        {
            throw std::out_of_range("vector::at - index out of range");
        }
        return operator[](n);
    }

    reference operator[](difference_type n)
    {
        return const_cast<reference>(static_cast<const self *>(this)->operator[](n));
    }

    const_reference operator[](difference_type n) const
    {
        return *(_start + n);
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
        return *_start;
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
        return *(_finish - 1);
    }

    pointer data()
    {
        return const_cast<pointer>(static_cast<const self *>(this)->data());
    }

    const_pointer data() const
    {
        return &*_start;
    }

    // iterators

    iterator begin()
    {
        return const_cast<iterator>(static_cast<const self *>(this)->begin());
    }

    const_iterator begin() const
    {
        return _start;
    }

    const_iterator cbegin() const
    {
        return _start;
    }

    iterator end()
    {
        return const_cast<iterator>(static_cast<const self *>(this)->end());
    }

    const_iterator end() const
    {
        return _finish;
    }

    const_iterator cend() const
    {
        return _finish;
    }

    // capacity

    bool empty() const
    {
        return _start == _finish;
    }

    size_type size() const
    {
        return _finish - _start;
    }

    size_type capacity() const
    {
        return _end_of_storage - _start;
    }

    constexpr size_type max_size() const
    {
        return SIZE_MAX / sizeof(T);
    }

    void reserve(size_type count)
    {
        assert(count >= 0 && count < max_size());
        // if (count > max_size())
        // {
        //     throw std::length_error("beyond the space allocated by system");
        // }
        if (count <= capacity())
        {
            return;
        }

        iterator old_first = begin();
        size_type old_count = size();
        initialize(count);
        _finish = uninitialized_copy_n(old_first, old_count, _start);
        clear(old_first, old_first + old_count);
        data_allocator::deallocate(&*old_first);
    }

    void shrink_to_fit()
    {
        size_type count = size();

        iterator old_first = begin();
        size_type old_count = size();
        initialize(count);
        _finish = uninitialized_copy_n(old_first, old_count, _start);
        clear(old_first, old_first + old_count);
        data_allocator::deallocate(&*old_first);
    }

    // modifier

    void clear()
    {
        clear(_start, _finish);
        _finish = _start;
    }

    iterator insert(const_iterator pos, const_reference val)
    {
        // return insert<const_reference>(pos, val);
        return emplace(pos, val);
    }

    iterator insert(const_iterator pos, T &&val)
    {
        // if (pos < begin() || pos > end())
        // {
        //     throw std::range_error("out of range");
        // }
        // difference_type index = pos - _start;
        // if (_finish == _end_of_storage)
        // {
        //     iterator old_start = _start;
        //     iterator old_finish = _finish;
        //     expand();

        //     iterator new_pos = _start + index;

        //     // uninitialized_copy(old_start, old_finish, _start);
        //     uninitialized_copy(old_start, const_cast<iterator>(pos), _start);
        //     _finish = uninitialized_copy(const_cast<iterator>(pos), old_finish, new_pos + 1);
        //     clear(old_start, old_finish);
        //     data_allocator::deallocate(&*old_start);
        //     construct(&*new_pos, std::forward<U>(val));
        // }
        // else
        // {
        //     iterator non_const_pos = const_cast<iterator>(pos);
        //     _finish = uninitialized_copy_backward(non_const_pos, _finish, non_const_pos + 1);
        //     construct(&*non_const_pos, std::forward<U>(val));
        // }
        // return _start + index;

        return emplace(pos, std::move(val));
    }

    // iterator insert(const_iterator pos, std::initializer_list<T> init)
    // {
    //     if (pos < begin() || pos > end())
    //     {
    //         throw std::range_error("out of range");
    //     }
    //     difference_type index = pos - _start;
    //     if (_finish + init.size() - 1 >= _end_of_storage)
    //     {
    //         iterator old_start = _start;
    //         iterator old_finish = _finish;
    //         initialize(size() + init.size());

    //         iterator new_pos = _start + index;

    //         // uninitialized_copy(old_start, old_finish, _start);
    //         uninitialized_copy(old_start, const_cast<iterator>(pos), _start);
    //         _finish =
    //             uninitialized_copy(const_cast<iterator>(pos), old_finish, new_pos + init.size());
    //         clear(old_start, old_finish);
    //         uninitialized_copy(init.begin(), init.end(), new_pos);
    //     }
    //     else
    //     {
    //         iterator non_const_pos = const_cast<iterator>(pos);
    //         _finish =
    //             uninitialized_copy_backward(non_const_pos, _finish, non_const_pos + init.size());
    //         uninitialized_copy(init.begin(), init.end(), non_const_pos);
    //     }
    //     return _start + index;
    // }

    template <typename... Args> iterator emplace(const_iterator pos, Args &&...args)
    {
        if (pos < begin() || pos > end())
        {
            throw std::range_error("out of range");
        }
        difference_type index = pos - _start;
        if (_finish == _end_of_storage)
        {
            iterator old_start = _start;
            iterator old_finish = _finish;
            expand();

            iterator new_pos = _start + index;

            // uninitialized_copy(old_start, old_finish, _start);
            uninitialized_copy(old_start, const_cast<iterator>(pos), _start);
            _finish = uninitialized_copy(const_cast<iterator>(pos), old_finish, new_pos + 1);
            clear(old_start, old_finish);
            data_allocator::deallocate(&*old_start);
            construct(&*new_pos, std::forward<Args>(args)...);
        }
        else
        {
            iterator non_const_pos = const_cast<iterator>(pos);
            _finish = uninitialized_copy_backward(non_const_pos, _finish, non_const_pos + 1);
            construct(&*non_const_pos, std::forward<Args>(args)...);
        }
        return _start + index;
    }

    iterator erase(iterator pos)
    {
        return erase((const_iterator)pos);
    }

    iterator erase(const_iterator pos)
    {
        // if (pos < begin() || pos > end())
        // {
        //     throw std::range_error("out of range");
        // }

        // iterator non_const_pos = const_cast<iterator>(pos);

        // _finish = uninitialized_copy(non_const_pos + 1, _finish, non_const_pos);
        // destroy(&*_finish);
        // return non_const_pos;
        return erase(pos, pos + 1);
    }

    iterator erase(iterator first, iterator last)
    {
        return erase((const_iterator)first, (const_iterator)last);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        if (first == last)
        {
            return const_cast<iterator>(last);
        }

        if (first > last || first < begin() || last > end())
        {
            throw std::range_error("out of range");
        }

        iterator non_const_first = const_cast<iterator>(first);
        iterator non_const_last = const_cast<iterator>(last);
        iterator old_finish = _finish;

        _finish = uninitialized_copy(non_const_last, _finish, non_const_first);
        clear(_finish, old_finish);
        return non_const_first;
    }

    void push_back(const_reference val)
    {
        emplace(_finish, val);
    }

    void push_back(T &&val)
    {
        emplace(_finish, std::move(val));
    }

    template <typename... Args> void emplace_back(Args &&...args)
    {
        emplace(_finish, std::forward<Args>(args)...);
    }

    void pop_back()
    {
        erase(_finish - 1);
    }

    void resize(size_type count)
    {
        assert(count >= 0 && count < max_size());
        if (count > capacity())
        {
            reserve(count);
        }

        if (count > size())
        {
            for (iterator cur = _finish; cur != _start + count; ++cur)
            {
                construct(&*cur);
            }
            _finish = _start + count;
        }
        else
        {
            for (iterator cur = _start + count; cur != _finish; ++cur)
            {
                destroy(&*cur);
            }
            _finish = _start + count;
        }
    }

    void resize(size_type count, const value_type &val)
    {
        assert(count >= 0 && count < max_size());
        if (count > capacity())
        {
            reserve(count);
        }

        if (count > size())
        {
            for (iterator cur = _finish; cur != _start + count; ++cur)
            {
                construct(&*cur, val);
            }
            _finish = _start + count;
        }
        else
        {
            for (iterator cur = _start + count; cur != _finish; ++cur)
            {
                destroy(&*cur);
            }
            _finish = _start + count;
        }
    }

    void swap(self &other)
    {
        if (this == &other)
        {
            return;
        }
        iterator tmp_start = other._start;
        iterator tmp_finish = other._finish;
        iterator tmp_end_of_storage = other._end_of_storage;

        other._start = _start;
        other._finish = _finish;
        other._end_of_storage = _end_of_storage;

        _start = tmp_start;
        _finish = tmp_finish;
        _end_of_storage = tmp_end_of_storage;
    }

  protected:
    void initialize(size_type count)
    {
        _start = data_allocator::allocate(count);
        _finish = _start;
        _end_of_storage = _start + count;
    }

    void fill_initialize(size_type count, const T &val)
    {
        initialize(count);
        uninitialized_fill_n(_start, count, val);
        _finish = _start + count;
    }

    void expand()
    {
        size_type count = size() * 2 + 1;
        initialize(count);
    }

    void zero_capacity()
    {
        clear(_start, _finish);
        data_allocator::deallocate(&*_start);
        _finish = _start;
        _end_of_storage = _start;
    }

    void clear(iterator first, iterator last)
    {
        for (iterator cur = first; cur != last; ++cur)
        {
            destroy(&*cur);
        }
    }

  protected:
    iterator _start;
    iterator _finish;
    iterator _end_of_storage;
};
} // namespace TS

#endif