#ifndef TS_UNINITIALIZED_HPP
#define TS_UNINITIALIZED_HPP

#include "ts_alloc.hpp"
#include <cstddef>

namespace TS
{
// uninitialized_copy
template <typename InputIter, typename ForwardIter>
inline ForwardIter uninitialized_copy(InputIter first, InputIter last, ForwardIter result)
{
    ForwardIter cur = result;
    try
    {
        for (; first != last; ++first, ++cur)
        {
            construct(&*cur, *first);
        }
    }
    catch (...)
    {
        for (ForwardIter it = result; it != cur; ++it)
        {
            destroy(&*it);
        }
        throw;
    }
    return cur;
}

template <typename InputIter, typename Size, typename ForwardIter>
inline ForwardIter uninitialized_copy_n(InputIter first, Size count, ForwardIter result)
{
    ForwardIter cur = result;
    try
    {
        for (; count > 0; --count, ++first, ++cur)
        {
            construct(&*cur, *first);
        }
    }
    catch (...)
    {
        for (ForwardIter it = result; it != cur; ++it)
        {
            destroy(&*it);
        }
        throw;
    }
    return cur;
}

template <typename InputIter, typename ForwardIter>
inline ForwardIter uninitialized_copy_backward(InputIter first, InputIter last, ForwardIter result)
{
    std::ptrdiff_t count = last - first;
    --last;
    try
    {
        for (ForwardIter cur = result + count - 1; cur != result - 1; --cur, --last)
        {
            construct(&*cur, *last);
        }
    }
    catch (...)
    {
        ForwardIter result_last = result + count;
        for (ForwardIter it = result; it != result_last; ++it)
        {
            destroy(&*it);
        }
        throw;
    }
    return result + count;
}

template <typename InputIter, typename Size, typename ForwardIter>
inline ForwardIter uninitialized_copy_backward_n(InputIter first, Size count, ForwardIter result)
{
    InputIter last = first + count - 1;
    try
    {
        for (ForwardIter cur = result + count - 1; cur != result - 1; --cur, --last)
        {
            construct(&*cur, *last);
        }
    }
    catch (...)
    {
        ForwardIter result_last = result + count;
        for (ForwardIter it = result; it != result_last; ++it)
        {
            detroy(&*it);
        }
        throw;
    }
    return result + count;
}

// uninitialized_fill
template <typename ForwardIter, typename T>
inline void uninitialized_fill(ForwardIter first, ForwardIter last, const T &val)
{
    try
    {
        for (ForwardIter cur = first; cur != last; ++cur)
        {
            construct(&*cur, val);
        }
    }
    catch (...)
    {
        for (ForwardIter it = first; it != last; ++it)
        {
            detroy(&*it);
        }
        throw;
    }
}

template <typename ForwardIter, typename Size, typename T>
inline void uninitialized_fill_n(ForwardIter first, Size count, const T &val)
{
    ForwardIter last = first + count;
    try
    {
        for (ForwardIter cur = first; cur != last; ++cur)
        {
            construct(&*cur, val);
        }
    }
    catch (...)
    {
        for (ForwardIter it = first; it != last; ++it)
        {
            destroy(&*it);
        }
    }
}

// template <typename ForwardIter> inline void uninitialized_break(ForwardIter first, ForwardIter last) noexcept
// {
//     for (; first != last; ++first)
//     {
//         destroy(&*first);
//     }
// }

// template <typename ForwardIter, typename Size> inline void uninitialized_break_n(ForwardIter first, Size count)
// noexcept
// {
//     for (ForwardIter last = first + count; first != last; ++first)
//     {
//         destroy(&*first);
//     }
// }

} // namespace TS

#endif