#ifndef MY_ALLOCATOR_H
#define MY_ALLOCATOR_H

#include <memory>
#include <vector>
#include <cstddef>
#include <type_traits>
#include <limits>
#include <stdexcept>

template <typename T, std::size_t ChunkSize = 10>
class my_allocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    template <typename U>
    struct rebind {
        using other = my_allocator<U, ChunkSize>;  
    };

    my_allocator() noexcept = default; 
    
    template <typename U>
    my_allocator(const my_allocator<U, ChunkSize>&) noexcept {}  
    
    my_allocator(const my_allocator&) noexcept = default;  
    
    ~my_allocator() {
        for (auto& chunk : chunks_) {
            for (size_type i = 0; i < chunk.used; ++i) {
                chunk.data[i].~T();
            }
            operator delete(chunk.data);
        }
    }

    pointer allocate(size_type n) {
        if (n == 0) return nullptr;
        
        // Поиск чанка с достаточным местом
        for (auto& chunk : chunks_) {
            if (chunk.size - chunk.used >= n) {
                pointer result = chunk.data + chunk.used;
                chunk.used += n;
                return result;
            }
        }
        
        // Создание нового чанка
        size_type new_size = std::max(ChunkSize, n);
        pointer new_memory = static_cast<pointer>(operator new(new_size * sizeof(T)));
        
        if (!new_memory) {
            throw std::bad_alloc();
        }
        
        chunks_.push_back({new_memory, new_size, 0});
        pointer result = chunks_.back().data;
        chunks_.back().used = n;
        return result;
    }

    // Способ 1: с (void)cast
    void deallocate(pointer p, size_type n) noexcept {
        // Пулловый аллокатор - память освобождается в деструкторе
        (void)p;  // Убираем предупреждение
        (void)n;  // Убираем предупреждение
    }
    
    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        ::new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(U* p) {
        p->~U();
    }

    size_type max_size() const noexcept {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }

    bool operator==(const my_allocator& other) const noexcept {  
        return this == &other;
    }

    bool operator!=(const my_allocator& other) const noexcept {  
        return !(*this == other);
    }

private:
    struct Chunk {
        pointer data;
        size_type size;
        size_type used;
    };
    
    std::vector<Chunk> chunks_;
};

#endif