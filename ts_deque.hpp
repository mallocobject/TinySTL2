#ifndef TS_DEQUE_HPP
#define TS_DEQUE_HPP

#include "ts_alloc.hpp"
#include "ts_iterator.hpp"
namespace TS
{
template <typename T, typename Ref, typename Ptr>
struct Deque_iterator : public _iterator<random_access_iterator_tag, T>
{
  public:
    using base_iterator = _iterator<random_access_iterator_tag, T>;
    using iterator = Deque_iterator<T, T &, T *>;
    using const_iterator = Deque_iterator<T, const T &, const T *>;
    using self = Deque_iterator<T, Ref, Ptr>;

    using typename base_iterator::difference_type;
    using typename base_iterator::iterator_category;
    using typename base_iterator::pointer;
    using typename base_iterator::reference;
    using typename base_iterator::value_type;
};
} // namespace TS

#endif