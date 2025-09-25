#ifndef TS_ALLOC_HPP
#define TS_ALLOC_HPP

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <utility>

namespace TS
{
#define THROW_BAD_ALLOC                                                                            \
    std::cout << "out of memory" << std::endl;                                                     \
    throw std::bad_alloc()

template <typename T> inline void construct(T *p)
{
    new (p) T();
}

template <typename T, typename U> inline void construct(T *p, const U &val)
{
    new (p) T(val);
}

template <typename T, typename U> inline void construct(T *p, U &&val)
{
    new (p) T(std::move(val));
}

// template <typename T, typename U> inline void reconstruct(T *p, const U &val)
// {
//     p->~T();
//     new (p) T(val);
// }

// template <typename T, typename U> inline void reconstruct(T *p, U &&val)
// {
//     p->~T();
//     new (p) T(std::forward<U>(val));
// }

// // Specialization for std::initializer_list to avoid lifetime issues
// template <typename T, typename U>
// inline void construct(T *p, std::initializer_list<U> il)
// {
//     new (p) T(il);
// }

// template <typename T, typename... Args> inline void construct(T *p, Args &&...args)
// {
//     std::cerr << "Constructing at " << p << " with " << sizeof...(args) << " arguments\n";
//     new (p) T(std::forward<Args>(args)...);
// }

template <typename T> inline void destroy(T *p)
{
    p->~T();
}

template <int inst> class malloc_alloc_template
{

  public:
    using pointer = void *;
    using size_type = std::size_t;

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

    static void *reallocate(void *p, std::size_t new_size)
    {
        void *result = realloc(p, new_size);
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
    static pointer oom_allocate(size_type n);
    static pointer oom_reallocate(pointer p, size_type new_size);
    static void (*malloc_alloc_oom_hander)();
};

template <int inst> void (*malloc_alloc_template<inst>::malloc_alloc_oom_hander)() = nullptr;

template <int inst> void *malloc_alloc_template<inst>::oom_allocate(std::size_t n)
{
    void (*my_malloc_hander)() = nullptr;
    void *result;
    while (true)
    {
        my_malloc_hander = malloc_alloc_oom_hander;
        if (nullptr == my_malloc_hander)
        {
            oom_allocate(n);
        }
        (*my_malloc_hander)();
        result = malloc(n);
        if (result)
        {
            return result;
        }
    }
}

template <int inst> void *malloc_alloc_template<inst>::oom_reallocate(void *p, std::size_t n)
{
    void (*my_malloc_hander)() = nullptr;
    void *result;
    while (true)
    {
        my_malloc_hander = malloc_alloc_oom_hander;
        if (nullptr == my_malloc_hander)
        {
            oom_reallocate(p, n);
        }
        (*my_malloc_hander)();
        result = realloc(p, n);
        if (result)
        {
            return result;
        }
    }
}

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

enum
{
    ALIGN = 8
};
enum
{
    MAX_BYTES = 128
};
enum
{
    NFREELISTS = 16
};

template <bool threads, int inst> class deafault_alloc_template
{
  public:
    using pointer = void *;
    using size_type = std::size_t;

  public:
    static pointer allocate(size_type n)
    {
    }

    static void deallocate(pointer p, size_type n)
    {
    }

    static pointer reallocate(pointer p, size_type old_size, size_type new_size);

  protected:
    static size_type round_up(size_type bytes)
    {
        return (bytes + ALIGN - 1) & (~ALIGN - 1);
    }

    union Obj {
        Obj *free_list_link;
        char *client_data;
    };

    static size_type free_list_index(size_type bytes)
    {
        return (bytes + ALIGN - 1) / ALIGN - 1;
    }

    static pointer refiil(size_type n);

    static char *chunk_alloc(size_type size, int &nobjs);

  protected:
    static Obj *free_list[]; // 二级指针
    static char *start_free;
    static char *end_free;
    static size_type heap_size;
};

template <bool threads, int inst>
typename deafault_alloc_template<threads, inst>::Obj *free_list[NFREELISTS];
// 初始化为0->nullptr

template <bool threads, int inst>
inline bool operator==(const deafault_alloc_template<threads, inst> &,
                       const deafault_alloc_template<threads, inst> &)
{
    return true;
}

template <bool threads, int inst>
void *deafault_alloc_template<threads, inst>::reallocate(pointer p, size_type old_size,
                                                         size_type new_size)
{
}

template <bool threads, int inst> void *deafault_alloc_template<threads, inst>::refiil(size_type n)
{
}

template <bool threads, int inst>
char *deafault_alloc_template<threads, inst>::chunk_alloc(size_type size, int &nobjs)
{
}
using alloc = deafault_alloc_template<false, 0>;

} // namespace TS

#endif