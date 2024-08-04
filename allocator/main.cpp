#include <iostream>
#include <vector>
#include <map>

using namespace std;

template <typename T>
struct logging_allocator {
    using value_type = T;

    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;

    template <typename U>
    struct rebind {
        using other = logging_allocator<U>;
    };

    logging_allocator():start(nullptr), end(nullptr), first_free(nullptr) {
        std::cout << "logging_allocator constructor " << std::endl;
        first_allocate =  true;
    };

    ~logging_allocator() = default;

    template <typename U>
    logging_allocator(const logging_allocator<U> &) {
    }

    T *allocate(std::size_t n) {
#ifndef USE_PRETTY
        std::cout << "allocate: [n = " << n << "]" << std::endl;
#else
        std::cout << __PRETTY_FUNCTION__ << "[n = " << n << "]" << std::endl;
#endif
        if (first_allocate ||(first_free == end)){  // если зашли первый раз или выделенная память закончилась
            first_free = reinterpret_cast<T *> (std::malloc(10 * sizeof(T)));
            if (first_allocate) start = first_free;
            end = first_free + 10 * sizeof(T);
        }
        auto p = first_free;
        if (!first_allocate){
            first_free +=sizeof(T);
        }else{
            first_allocate = false;
        }

        if (!p)
            throw std::bad_alloc();
        return reinterpret_cast<T *>(p);
    }

    void deallocate(T *p, std::size_t n) {
#ifndef USE_PRETTY
        std::cout << "deallocate: [n  = " << n << "] " << std::endl;
#else
        std::cout << __PRETTY_FUNCTION__ << "[n = " << n << "]" << std::endl;
#endif
        std::free(p);
    }

    template <typename U, typename... Args>
    void construct(U *p, Args &&...args) {
#ifndef USE_PRETTY
        std::cout << "construct" << std::endl;
#else
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        new (p) U(std::forward<Args>(args)...);
    };

    template <typename U>
    void destroy(U *p) {
#ifndef USE_PRETTY
        std::cout << "destroy" << std::endl;
#else
        std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif
        p->~U();
    }

    T* start;           // начало выделенного блока памяти
    T* end;             // конец выделенного блока памяти
    T* first_free;      // первый свободный элемент памяти
    bool first_allocate;
};


int main()
{
    auto v = std::vector<int, logging_allocator<int>>{};
        //v.reserve(5);
        for (int i = 0; i < 6; ++i) {
            std::cout << "vector size = " << v.size() << std::endl;
            v.emplace_back(i);
            std::cout << std::endl;
        }

        auto m = std::map<
            int,
            float,
            std::less<int>,
            logging_allocator<
                std::pair<
                    const int, float>>>{};

        for (int i = 0; i < 1; ++i) {
            m[i] = static_cast<float>(i);
        }
    return 0;
}
