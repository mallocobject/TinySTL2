#ifndef TS_LIST_HPP
#define TS_LIST_HPP

#include "ts_alloc.hpp"
#include "ts_iterator.hpp"
#include <cstddef>
#include <initializer_list>
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
    }

    List_node(const T &data, self *prev, self *next) : _data(data), _prev(prev), _next(next)
    {
    }

    List_node(T &&data, self *prev, self *next) : _data(std::move(data)), _prev(prev), _next(next)
    {
    }

    template <typename... Args>
    List_node(self *prev, self *next, Args &&...args)
        : _data(std::forward<Args>(args)...), _prev(prev), _next(next)
    {
    }

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
    List_iterator() : _node(nullptr)
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

  public:
    Node *_node;
};

template <typename T, typename Alloc = malloc_alloc> class list
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

    list(size_type count)
    {
        iterator result = _node->_next;
        while (count)
        {
            result = insert(result, T());
        }
    }

    list(size_type count, const T &val)
    {
        iterator result = _node->_next;
        while (count)
        {
            result = insert(result, val);
        }
    }

    list(const self &other)
    {
    }

    list(self &&other) noexcept
    {
    }

    list(std::initializer_list<T> init)
    {
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
        return _node->_next;
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
        return _node->_prev;
    }

    iterator begin()
    {
        return _node->_next;
    }

    const_iterator begin() const
    {
        return _node->_next;
    }

    const_iterator cbegin() const noexcept
    {
        return _node->_next;
    }

    iterator end()
    {
        return _node;
    }

    const_iterator end() const
    {
        return _node;
    }

    const_iterator cend() const noexcept
    {
        return _node;
    }

    iterator insert(const_iterator pos, const T &val)
    {
        Node *new_node = get_node();
        construct(new_node, val, pos._node->_prev, pos._node);

        pos._node->_prev = new_node;
        new_node->_prev->_next = new_node;

        return iterator(new_node);
    }

    bool empty() const
    {
        return _node->_next == _node;
    }

    iterator insert(const_iterator pos, T &&val)
    {
        Node *new_node = get_node();
        construct(new_node, std::forward<T>(val), pos._node->_prev, pos._node);

        pos._node->_prev = new_node;
        new_node->_prev->_next = new_node;

        return iterator(new_node);
    }

    iterator insert(const_iterator pos, size_type count, const T &val)
    {
        iterator result = const_iterator_to_iterator(pos);
        while (count-- > 0)
        {
            result = insert(result, val);
        }

        return result;
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
        iterator result = const_iterator_to_iterator(pos);
        for (auto iter = init.begin(); iter != init.end(); ++iter)
        {
            result = insert(result, *iter);
        }

        return result;
    }

    template <typename... Args> iterator emplace(const_iterator pos, Args &&...args)
    {
        Node *new_node = get_node();
        construct(new_node, pos._node->_prev, pos._node, std::forward<Args>(args)...);

        pos._node->_prev = new_node;
        new_node->_prev->_next = new_node;

        return iterator(new_node);
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

  protected:
    Node *get_node()
    {
        return data_allocator::allocate();
    }
    void put_node(Node *p)
    {
        data_allocator::deallocate(p);
    }

    iterator const_iterator_to_iterator(const_iterator cit)
    {
        return iterator(cit._node);
    }

  protected:
    Node *_node;
};

} // namespace TS

#endif