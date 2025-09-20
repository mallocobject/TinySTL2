#ifndef TS_ALLOC_HPP
#define TS_ALLOC_HPP

#include <cstddef>
#include <cstdlib>
#include <iostream>

namespace TS
{
#define THROW_BAD_ALLOC                                                                            \
    std::cout << "out of memory" << std::endl;                                                     \
    throw std::bad_alloc()

template <typename T> inline void construct(T *p)
{
    new (p) T();
}

template <typename T1, typename T2> inline void construct(T1 *p, const T2 &val)
{
    new (p) T1(val);
}

template <typename T> inline void destroy(T *p)
{
    p->~T();
}

template <int inst> class malloc_alloc_template
{
  public:
    static void *allocate(std::size_t size)
    {
        void *result = malloc(size);
        if (nullptr == result)
        {
            // 暂时如此处理
            THROW_BAD_ALLOC;
        }
        return result;
    }

    static void *reallocate(void *p, std::size_t size)
    {
        void *result = realloc(p, size);
        if (nullptr == result)
        {
            // 暂时如此处理
            THROW_BAD_ALLOC;
        }
        return result;
    }

    static void deallocate(void *p)
    {
        free(p);
    }

  protected:
    // static pointer oom_allocate(size_type n);
    // static pointer oom_reallocate(pointer p, size_type new_size);
    // static void oom_deallocate(pointer p);
};

using malloc_alloc = malloc_alloc_template<0>;

template <typename T, class Alloc> class simple_alloc
{
  public:
    static T *allocate(std::size_t count)
    {
        return 0 == count ? nullptr : (T *)Alloc::allocate(count * sizeof(T));
    }

    static T *allocate()
    {
        return (T *)Alloc::allocate(sizeof(T));
    }

    static T *reallocate(T *p, std::size_t count)
    {
        T *result = nullptr;
        if (0 == count)
        {
            Alloc::deallocate(p);
        }
        else
        {
            result = (T *)Alloc::reallocate(p, count * sizeof(T));
        }
        return result;
    }

    static void deallocate(T *p)
    {
        Alloc::deallocate(p);
    }
};

} // namespace TS

#endif