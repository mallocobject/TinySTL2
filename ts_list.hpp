#ifndef TS_LIST_HPP
#define TS_LIST_HPP

#include "ts_iterator.hpp"

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
};

} // namespace TS

#endif