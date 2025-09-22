#ifndef TS_ITERATOR_HPP
#define TS_ITERATOR_HPP

#include <cstddef>
#include <ctime>

namespace TS
{
template <typename Category, typename T, typename Distance = std::ptrdiff_t, typename Pointer = T *,
          typename Reference = T &>
struct iterator
{
  public:
    using iterator_category = Category;
    using value_type = T;
    using difference_type = Distance;
    using pointer = Pointer;
    using reference = Reference;
};

struct input_iterator_tag
{
};

struct output_iterator_tag
{
};

struct forward_iterator_tag : public input_iterator_tag
{
};

struct bidirectional_iterator_tag : public forward_iterator_tag
{
};

struct random_access_iterator_tag : public bidirectional_iterator_tag
{
};

template <typename Iter> struct iterator_traits
{
  public:
    using difference_type = typename Iter::difference_type;
    using value_type = typename Iter::value_type;
    using pointer = typename Iter::pointer;
    using reference = typename Iter::reference;
    using iterator_category = typename Iter::iterator_category;
};

template <typename T> struct iterator_traits<T *>
{
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;
    using iterator_category = random_access_iterator_tag;
};

template <typename T> struct iterator_traits<const T *>
{
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = const T *;
    using reference = const T &;
    using iterator_category = random_access_iterator_tag;
};

template <typename Iter>
inline typename iterator_traits<Iter>::iterator_category iterator_category(const Iter &)
{
    using category = typename iterator_traits<Iter>::iterator_category;
    return category();
}

template <typename Iter>
inline typename iterator_traits<Iter>::difference_type *distance_type(const Iter &)
{
    return nullptr;
}

template <typename Iter> inline typename iterator_traits<Iter>::value_type *value_type(const Iter &)
{
    return nullptr;
}

} // namespace TS

#endif