#ifndef MY_ALLOCATOR_H
#define MY_ALLOCATOR_H

#include <memory>
#include <vector>
#include <cstddef>
#include <type_traits>
#include <limits>
#include <stdexcept>

// Шаблонный класс аллокатора с параметрами:
template <typename T, std::size_t ChunkSize = 10>
class my_allocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;  // Тип для размеров
    using difference_type = std::ptrdiff_t; // Тип для разницы указателей
    
    template <typename U>
    struct rebind {
        using other = my_allocator<U, ChunkSize>;
    };

    // Конструктор по умолчанию
    my_allocator() noexcept = default; 
    
    // Конструктор копирования для другого типа 
    template <typename U>
    my_allocator(const my_allocator<U, ChunkSize>&) noexcept {}  
    
    // Конструктор копирования
    my_allocator(const my_allocator&) noexcept = default;  
    
    // Деструктор
    ~my_allocator() {
        for (auto& chunk : chunks_) {
            for (size_type i = 0; i < chunk.used; ++i) {
                chunk.data[i].~T();  // Вызов деструктора для каждого объекта
            }
            // Освобождаем сырую память чанка
            operator delete(chunk.data);
        }
        // Вектор chunks автоматически уничтожится
    }

    // Основной метод выделения памяти
    pointer allocate(size_type n) {
        if (n == 0) return nullptr;
        
        // Поиск чанка с достаточным местом среди уже существующих
        for (auto& chunk : chunks_) {
            // Если в текущем чанке достаточно свободного места
            if (chunk.size - chunk.used >= n) {
                // Возвращаем указатель на начало свободной области
                pointer result = chunk.data + chunk.used;
                chunk.used += n;  // Увеличиваем счетчик использованных элементов
                return result;
            }
        }
        
        // Если подходящего чанка не найдено - создаем новый
        size_type new_size = std::max(ChunkSize, n);
        // Выделяем сырую память для чанка
        pointer new_memory = static_cast<pointer>(operator new(new_size * sizeof(T)));
        if (!new_memory) {
            throw std::bad_alloc();
        }
        
        // Добавляем новый чанк в вектор
        chunks_.push_back({new_memory, new_size, 0});
        pointer result = chunks_.back().data;
        chunks_.back().used = n;  // Помечаем память как использованную
        return result;
    }

    // Метод освобождения памяти
    void deallocate(pointer p, size_type n) noexcept {
        (void)p;
        (void)n;
    }
    
    // Метод для конструирования объекта в выделенной памяти
    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        // вызывает конструктор в уже выделенной памяти
        ::new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }

    // Метод для явного вызова деструктора объекта
    template <typename U>
    void destroy(U* p) {
        p->~U();  // вызов деструктора
    }

    size_type max_size() const noexcept {
        return std::numeric_limits<size_type>::max() / sizeof(T);
    }

    // Операторы сравнения аллокаторов

    bool operator==(const my_allocator& other) const noexcept {  
        return this == &other;  // Сравнение по идентичности объектов
    }

    bool operator!=(const my_allocator& other) const noexcept {  
        return !(*this == other);  // Противоположное равенству
    }

private:
    // Структура для представления блока памяти
    struct Chunk {
        pointer data;      // Указатель на начало памяти чанка
        size_type size;    // Общий размер чанка 
        size_type used;    // Количество использованных элементов в чанке
    };
    
    // Вектор для хранения всех выделенных чанков
    std::vector<Chunk> chunks_;
};

#endif