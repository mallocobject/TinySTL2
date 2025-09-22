#ifndef TS_LIST_HPP
#define TS_LIST_HPP

#include "ts_alloc.hpp"
#include "ts_iterator.hpp"
#include <cstddef>
#include <initializer_list>

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

  public:
    List_node *_prev;
    List_node *_next;
    T _data;
};

template <typename T, typename Ref, typename Ptr>
struct List_iterator : public iterator<bidirectional_iterator_tag, T>
{
  public:
    using base_iterator = iterator<bidirectional_iterator_tag, T>;
    using iterator = List_iterator<T, T &, T *>;
    using const_iterator = List_iterator<T, const T &, const T *>;
    using self = List_iterator<T, Ref, Ptr>;

    using typename iterator::difference_type;
    using typename iterator::iterator_category;
    using typename iterator::pointer;
    using typename iterator::reference;
    using typename iterator::value_type;

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
    using data_allocator = simple_alloc<T, Alloc>;
    using self = list<Node, Alloc>;

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
    }

    list(size_type count, const T &val)
    {
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

    template <typename... Args> iterator emplace(const_iterator pos, Args &&...args)
    {
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

  protected:
    Node *_node;
};

} // namespace TS

#endif