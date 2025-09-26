#ifndef TS_LIST_HPP
#define TS_LIST_HPP

#include "ts_alloc.hpp"
#include "ts_iterator.hpp"
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <limits>
#include <stdexcept>
#include <utility>

namespace TS
{
template <typename T> struct List_node
{
  protected:
    using self = List_node<T>;

  public:
    List_node() : _data(T{}), _prev(nullptr), _next(nullptr)
    {
    }

    List_node(const T &data) : _data(data), _prev(nullptr), _next(nullptr)
    {
        // std::cout << "-------------" << std::endl;
    }

    List_node(T &&data) : _data(std::move(data)), _prev(nullptr), _next(nullptr)
    {
        // std::cout << "&&&&&&&&&&&&&" << std::endl;
    }

    // template <typename... Args>
    // List_node(Args &&...args) : _data(std::forward<Args>(args)...), _prev(nullptr),
    // _next(nullptr)
    // {
    // }

  public:
    List_node *_prev;
    List_node *_next;
    T _data;
};

template <typename T, typename Ref, typename Ptr>
struct List_iterator : public _iterator<bidirectional_iterator_tag, T>
{
  public:
    using base_iterator = _iterator<bidirectional_iterator_tag, T>;
    using iterator = List_iterator<T, T &, T *>;
    using const_iterator = List_iterator<T, const T &, const T *>;
    using self = List_iterator<T, Ref, Ptr>;

    using typename base_iterator::difference_type;
    using typename base_iterator::iterator_category;
    using typename base_iterator::pointer;
    using typename base_iterator::reference;
    using typename base_iterator::value_type;

    using Node = List_node<T>;

  public:
    List_iterator() noexcept : _node(nullptr)
    {
    }

    List_iterator(Node *node) : _node(node)
    {
    }

    List_iterator(const iterator &other) : _node(other._node)
    {
    }

    reference operator*() const
    {
        return _node->_data;
    }

    pointer operator->() const
    {
        return &(operator*());
    }

    self &operator++()
    {
        _node = _node->_next;
        return *this;
    }

    self operator++(int)
    {
        self tmp = *this;
        _node = _node->_next;
        return tmp;
    }

    self &operator--()
    {
        _node = _node->_prev;
        return *this;
    }

    self operator--(int)
    {
        self tmp = *this;
        _node = _node->_prev;
        return tmp;
    }

    template <typename OtherRef, typename OtherPtr>
    bool operator==(const List_iterator<T, OtherRef, OtherPtr> &other) const
    {
        return _node == other._node;
    }

    template <typename OtherRef, typename OtherPtr>
    bool operator!=(const List_iterator<T, OtherRef, OtherPtr> &other) const
    {
        return _node != other._node;
    }

  public:
    Node *_node;
};

template <typename T, typename Alloc = alloc> class list
{
  public:
    using value_type = T;
    using size_type = std::size_t;
    using difference = std::ptrdiff_t;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using iterator = List_iterator<T, T &, T *>;
    using const_iterator = List_iterator<T, const T &, const T *>;
    using Node = List_node<T>;

  protected:
    using data_allocator = simple_alloc<Node, Alloc>;
    using self = list<T, Alloc>;

  public:
    ~list()
    {

        clear();
        put_node(_node);
    }

    list()
    {
        _node = get_node();
        _node->_prev = _node;
        _node->_next = _node;
    }

    list(size_type count) : list()
    {
        while (count-- > 0)
        {
            insert(end(), T());
        }
    }

    list(size_type count, const T &val) : list()
    {
        while (count-- > 0)
        {
            insert(end(), val);
        }
    }

    list(const self &other) : list()
    {
        for (const_iterator it = other.begin(); it != other.end(); ++it)
        {
            insert(end(), *it);
        }
    }

    list(self &&other) noexcept : _node(other._node), _size(other._size)
    {
        other._size = 0;
        other._node = other.get_node();
        other._node->_next = other._node;
        other._node->_prev = other._node;
    }

    list(std::initializer_list<T> init) : list()
    {
        for (auto iter = init.begin(); iter != init.end(); ++iter)
        {
            insert(end(), *iter);
        }
    }

    list &operator=(const self &other)
    {
        if (this == &other)
        {
            return *this;
        }
        for (const_iterator it = other.begin(); it != other.end(); ++it)
        {
            insert(end(), *it);
        }

        return *this;
    }

    list &operator=(self &&other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }
        _node = other._node;
        _size = other._size;

        other._size = 0;
        other._node = other.get_node();
        other._node->_next = other._node;
        other._node->_prev = other._node;

        return *this;
    }

    list &operator=(std::initializer_list<T> init)
    {
        for (auto iter = init.begin(); iter != init.end(); ++iter)
        {
            insert(end(), *iter);
        }

        return *this;
    }

    void assign(std::initializer_list<T> init)
    {
        clear();
        for (auto iter = init.begin(); iter != init.end(); ++iter)
        {
            insert(end(), *iter);
        }
    }

    reference front()
    {
        return const_cast<reference>(static_cast<const self *>(this)->front());
    }

    const_reference front() const
    {
        if (empty())
        {
            throw std::range_error("empty list");
        }
        return _node->_next->_data;
    }

    reference back()
    {
        return const_cast<reference>(static_cast<const self *>(this)->back());
    }

    const_reference back() const
    {
        if (empty())
        {
            throw std::range_error("empty list");
        }
        return _node->_prev->_data;
    }

    iterator begin()
    {
        return iterator(_node->_next);
    }

    const_iterator begin() const
    {
        return const_iterator(_node->_next);
    }

    const_iterator cbegin() const noexcept
    {
        return const_iterator(_node->_next);
    }

    iterator end()
    {
        return iterator(_node);
    }

    const_iterator end() const
    {
        return iterator(_node);
    }

    const_iterator cend() const noexcept
    {
        return const_iterator(_node);
    }

    bool empty() const
    {
        return _node->_next == _node;
    }

    size_type size() const
    {
        return _size - 1;
    }
    size_type max_size() const
    {
        return std::numeric_limits<size_type>::max() / sizeof(Node);
    }

    void clear()
    {
        Node *cur = _node->_next;
        while (cur != _node)
        {
            Node *tmp = cur;
            cur = cur->_next;
            destroy(tmp);
            put_node(tmp);
        }
        _node->_next = _node;
        _node->_prev = _node;
    }

    iterator insert(const_iterator pos, const T &val)
    {
        Node *new_node = get_node();
        construct(new_node, val);
        new_node->_prev = pos._node->_prev;
        new_node->_next = pos._node,

        new_node->_prev->_next = new_node;
        pos._node->_prev = new_node;

        return iterator(new_node);
    }

    iterator insert(const_iterator pos, T &&val)
    {
        Node *new_node = get_node();
        construct(new_node, std::move(val));
        new_node->_prev = pos._node->_prev;
        new_node->_next = pos._node,

        pos._node->_prev = new_node;
        new_node->_prev->_next = new_node;

        return iterator(new_node);
    }

    iterator insert(const_iterator pos, size_type count, const T &val)
    {
        iterator result(pos._node->_prev);
        while (count-- > 0)
        {
            insert(pos, val);
        }

        return iterator(result._node->_next);
    }

    // template <typename InputIter>
    // iterator insert(const_iterator pos, InputIter first, InputIter last)
    // {
    //     iterator result = const_iterator_to_iterator(pos);
    //     for (; first != last; ++first)
    //     {
    //         result = insert(result, *first);
    //     }

    //     return result;
    // }

    iterator insert(const_iterator pos, std::initializer_list<T> init)
    {
        iterator result(pos._node->_prev);
        for (auto iter = init.begin(); iter != init.end(); ++iter)
        {
            insert(pos, *iter);
        }

        return iterator(result._node->_next);
    }

    template <typename... Args> iterator emplace(const_iterator pos, Args &&...args)
    {
        Node *new_node = get_node();
        construct(new_node, std::forward<Args>(args)...);
        new_node->_prev = pos._node->_prev;
        new_node->_next = pos._node,

        pos._node->_prev = new_node;
        new_node->_prev->_next = new_node;

        return iterator(new_node);
    }

    iterator erase(iterator pos)
    {
        iterator result(pos._node->_next);
        pos._node->_prev->_next = pos._node->_next;
        pos._node->_next->_prev = pos._node->_prev;

        destroy(pos._node);
        put_node(pos._node);

        return result;
    }

    iterator erase(const_iterator pos)
    {
        return erase(static_cast<iterator>(pos));
    }

    iterator erase(iterator first, iterator last)
    {
        iterator result(first._node->_prev);
        while (first != last)
        {
            iterator tmp = first._node->_next;
            erase(first);
            first = tmp;
        }

        return iterator(result._node->_next);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        return erase(static_cast<iterator>(first), static_cast<iterator>(last));
    }

    void push_back(const T &val)
    {
        insert(end(), val);
    }

    void push_back(T &&val)
    {
        insert(end(), std::move(val));
    }

    template <typename... Args> void emplace_back(Args &&...args)
    {
        iterator result = emplace(end(), std::forward<Args>(args)...);
    }

    // template <typename... Args> reference emplace_back(Args &&...args)
    // {
    //     iterator result = emplace(end(), std::forward<Args>(args)...);
    //     return result._node->_data;
    // }

    void pop_back()
    {
        if (empty())
        {
            throw std::range_error("empty list");
        }

        erase(--end());
    }

    void push_front(const T &val)
    {
        insert(begin(), val);
    }

    void push_front(T &&val)
    {
        insert(begin(), std::move(val));
    }

    template <typename... Args> void emplace_front(Args &&...args)
    {
        iterator result = emplace(begin(), std::forward<Args>(args)...);
    }

    // template <typename... Args> reference emplace_front(Args &&...args)
    // {
    //     iterator result = emplace(begin(), std::forward<Args>(args)...);
    //     return result._node->_data;
    // }

    void pop_front()
    {
        if (empty())
        {
            throw std::range_error("empty list");
        }

        erase(begin());
    }

    void resize(size_type count)
    {
        resize(count, T());
    }

    void resize(size_type count, const value_type &val)
    {
        assert(count >= 0);
        if (count == size())
        {
            return;
        }
        else if (count < size())
        {
            iterator first = begin();
            while (count-- > 0)
            {
                ++first;
            }

            erase(first, end());
        }
        else
        {
            insert(end(), count - size(), val);
        }
    }

    void swap(self &other)
    {
        Node *tmp_node = other._node;
        size_type tmp_size = other._size;

        other._node = _node;
        other._size = _size;

        _node = tmp_node;
        _size = tmp_size;
    }

  protected:
    Node *get_node()
    {
        _size++;
        return data_allocator::allocate();
    }
    void put_node(Node *p)
    {
        _size--;
        data_allocator::deallocate(p);
    }

    iterator const_iterator_to_iterator(const_iterator cit)
    {
        return iterator(cit._node);
    }

  protected:
    Node *_node;
    size_type _size = 0;
};

} // namespace TS

#endif