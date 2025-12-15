#ifndef MY_CONTAINER_H
#define MY_CONTAINER_H

#include <memory>
#include <iterator>
#include <initializer_list>
#include <type_traits>

// Шаблонный класс контейнера
template <typename T, typename Allocator = std::allocator<T>>
class my_container {
private:
    struct Node {
        T data;      // Данные узла
        Node* next;  // Указатель на следующий узел
        
        //Шаблонный констурктор позволяет конструировать данные с любым количеством аргументов
        template <typename... Args>
        Node(Args&&... args) 
            : data(std::forward<Args>(args)...),  // Передача аргументов конструктору T
              next(nullptr) {}                    // Следующий узел инициализируется nullptr
    };
    
    using node_allocator_type = typename std::allocator_traits<Allocator>::
        template rebind_alloc<Node>;
    using node_traits = std::allocator_traits<node_allocator_type>;
    
    Node* head_;
    Node* tail_;
    size_t size_;               // Количество элементов в контейнере
    node_allocator_type allocator_;  // Аллокатор для выделения памяти под узлы

public:
    // Класс неконстантного итератора
    class iterator {
    public:
        // Определения типов
        using iterator_category = std::forward_iterator_tag;  // Однонаправленный итератор
        using value_type = T;           // Тип значения
        using difference_type = std::ptrdiff_t;  // Тип для разности итераторов
        using pointer = T*;             // Указатель на элемент
        using reference = T&;           // Ссылка на элемент

        // Конструктор итератора
        iterator(Node* node = nullptr) : current_(node) {}
        
        // Оператор разыменования 
        reference operator*() const { return current_->data; }
        
        // Оператор доступа к членам через указатель
        pointer operator->() const { return &current_->data; }
        
        // Префиксный инкремент 
        iterator& operator++() {
            current_ = current_->next;  // Переход к следующему узлу
            return *this;
        }
        
        // Постфиксный инкремент
        iterator operator++(int) {
            iterator tmp = *this;  // Сохраняем текущее состояние
            ++(*this);             // Используем префиксный инкремент
            return tmp;            // Возвращаем старое состояние
        }
        
        // Операторы сравнения
        bool operator==(const iterator& other) const { 
            return current_ == other.current_; 
        }
        
        bool operator!=(const iterator& other) const { 
            return !(*this == other); 
        }
        
    private:
        Node* current_;  
        friend class my_container;
    };
    
    // Класс константного итератора
    class const_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const T;  // Константный тип значения
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;    // Константный указатель
        using reference = const T&;  // Константная ссылка

        // Конструкторы
        const_iterator(const Node* node = nullptr) : current_(node) {}
        const_iterator(const iterator& it) : current_(it.current_) {}
        
        reference operator*() const { return current_->data; }
        pointer operator->() const { return &current_->data; }
        
        const_iterator& operator++() {
            current_ = current_->next;
            return *this;
        }
        
        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        bool operator==(const const_iterator& other) const { 
            return current_ == other.current_; 
        }
        
        bool operator!=(const const_iterator& other) const { 
            return !(*this == other); 
        }
        
    private:
        const Node* current_;  // Константный указатель на узел
    };
    
    // Конструктор по умолчанию
    my_container() : head_(nullptr), tail_(nullptr), size_(0) {}
    
    // Конструктор с аллокатором
    explicit my_container(const Allocator& alloc) 
        : head_(nullptr), tail_(nullptr), size_(0), allocator_(alloc) {}
    
    // Конструктор со списком инициализации
    my_container(std::initializer_list<T> init, const Allocator& alloc = Allocator())
        : head_(nullptr), tail_(nullptr), size_(0), allocator_(alloc) {
        // Добавляем все элементы из initializer_list
        for (const auto& item : init) {
            push_back(item);
        }
    }

    // Конструктор копирования
    my_container(const my_container& other) 
        : head_(nullptr), tail_(nullptr), size_(0), 
          // Копируем аллокатор с учетом политики копирования
          allocator_(node_traits::select_on_container_copy_construction(other.allocator_)) {
        // Последовательно копируем все элементы из другого контейнера
        for (const auto& item : other) {
            push_back(item);
        }
    }
    
    // Конструктор перемещения
    my_container(my_container&& other) noexcept
        : head_(other.head_), tail_(other.tail_), 
          size_(other.size_), allocator_(std::move(other.allocator_)) {
        // Обнуляем указатели в перемещаемом объекте
        other.head_ = other.tail_ = nullptr;
        other.size_ = 0;
    }
    
    // Оператор присваивания копированием
    my_container& operator=(const my_container& other) {
        // Проверка на самоприсваивание
        if (this != &other) {
            clear();  // Освобождаем текущие ресурсы
            allocator_ = other.allocator_;  // Копируем аллокатор
            // Копируем элементы
            for (const auto& item : other) {
                push_back(item);
            }
        }
        return *this;
    }
    
    // Оператор присваивания перемещением
    my_container& operator=(my_container&& other) noexcept {
        if (this != &other) {
            clear();  // Освобождаем текущие ресурсы
            // Перемещаем ресурсы из другого контейнера
            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;
            allocator_ = std::move(other.allocator_);
            // Обнуляем указатели в перемещаемом объекте
            other.head_ = other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
    
    ~my_container() {
        clear(); 
    }
    
    // Добавление элемента в конец 
    void push_back(const T& value) {
        // Выделяем память для нового узла
        Node* new_node = node_traits::allocate(allocator_, 1);
        try {
            // Конструируем узел в выделенной памяти
            node_traits::construct(allocator_, new_node, value);
        } catch (...) {
            // В случае исключения при конструировании освобождаем память
            node_traits::deallocate(allocator_, new_node, 1);
            throw;  // Пробрасываем исключение дальше
        }
        
        // Добавляем узел в список
        if (!head_) {
            // Если список пуст - новый узел становится и головой и хвостом
            head_ = tail_ = new_node;
        } else {
            // Иначе добавляем в конец
            tail_->next = new_node;
            tail_ = new_node;
        }
        ++size_;  // Увеличиваем счетчик элементов
    }
    
    // Добавление элемента в конец 
    template <typename... Args>
    void emplace_back(Args&&... args) {
        Node* new_node = node_traits::allocate(allocator_, 1);
        try {
            // Конструируем узел, передавая аргументы напрямую конструктору T
            node_traits::construct(allocator_, new_node, std::forward<Args>(args)...);
        } catch (...) {
            node_traits::deallocate(allocator_, new_node, 1);
            throw;
        }
        
        if (!head_) {
            head_ = tail_ = new_node;
        } else {
            tail_->next = new_node;
            tail_ = new_node;
        }
        ++size_;
    }
    
    // Очистка контейнера
    void clear() noexcept {
        while (head_) {
            Node* next = head_->next;        // Сохраняем указатель на следующий узел
            node_traits::destroy(allocator_, head_);  // Вызываем деструктор узла
            node_traits::deallocate(allocator_, head_, 1);  // Освобождаем память
            head_ = next;                     // Переходим к следующему узлу
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }
    
    // Получение количества элементов
    size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }

    // Неконстантные итераторы
    iterator begin() noexcept { return iterator(head_); }
    iterator end() noexcept { return iterator(nullptr); }  
    
    // Константные итераторы (для константных объектов)
    const_iterator begin() const noexcept { return const_iterator(head_); }
    const_iterator end() const noexcept { return const_iterator(nullptr); }
    
    // Явно константные итераторы (можно вызывать у неконстантных объектов)
    const_iterator cbegin() const noexcept { return const_iterator(head_); }
    const_iterator cend() const noexcept { return const_iterator(nullptr); }
};

#endif