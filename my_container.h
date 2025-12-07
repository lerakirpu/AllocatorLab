#ifndef MY_CONTAINER_H
#define MY_CONTAINER_H

#include <memory>
#include <iterator>
#include <initializer_list>
#include <type_traits>

template <typename T, typename Allocator = std::allocator<T>>
class my_container {
private:
    struct Node {
        T data;
        Node* next;
        
        template <typename... Args>
        Node(Args&&... args) 
            : data(std::forward<Args>(args)...), next(nullptr) {}
    };
    
    using node_allocator_type = typename std::allocator_traits<Allocator>::
        template rebind_alloc<Node>;
    using node_traits = std::allocator_traits<node_allocator_type>;
    
    Node* head_;
    Node* tail_;
    size_t size_;
    node_allocator_type allocator_;

public:
    // Итератор
    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator(Node* node = nullptr) : current_(node) {}
        
        reference operator*() const { return current_->data; }
        pointer operator->() const { return &current_->data; }
        
        iterator& operator++() {
            current_ = current_->next;
            return *this;
        }
        
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        bool operator==(const iterator& other) const { 
            return current_ == other.current_; 
        }
        
        bool operator!=(const iterator& other) const { 
            return !(*this == other); 
        }
        
    private:
        Node* current_;
    };
    
    class const_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

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
        const Node* current_;
    };

    // Конструкторы
    my_container() : head_(nullptr), tail_(nullptr), size_(0) {}
    
    explicit my_container(const Allocator& alloc) 
        : head_(nullptr), tail_(nullptr), size_(0), allocator_(alloc) {}
    
    my_container(std::initializer_list<T> init, const Allocator& alloc = Allocator())
        : head_(nullptr), tail_(nullptr), size_(0), allocator_(alloc) {
        for (const auto& item : init) {
            push_back(item);
        }
    }
    
    // Правило пяти
    my_container(const my_container& other) 
        : head_(nullptr), tail_(nullptr), size_(0), 
          allocator_(node_traits::select_on_container_copy_construction(other.allocator_)) {
        for (const auto& item : other) {
            push_back(item);
        }
    }
    
    my_container(my_container&& other) noexcept
        : head_(other.head_), tail_(other.tail_), 
          size_(other.size_), allocator_(std::move(other.allocator_)) {
        other.head_ = other.tail_ = nullptr;
        other.size_ = 0;
    }
    
    my_container& operator=(const my_container& other) {
        if (this != &other) {
            clear();
            allocator_ = other.allocator_;
            for (const auto& item : other) {
                push_back(item);
            }
        }
        return *this;
    }
    
    my_container& operator=(my_container&& other) noexcept {
        if (this != &other) {
            clear();
            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;
            allocator_ = std::move(other.allocator_);
            other.head_ = other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
    
    ~my_container() {
        clear();
    }
    
    // Основные методы
    void push_back(const T& value) {
        Node* new_node = node_traits::allocate(allocator_, 1);
        try {
            node_traits::construct(allocator_, new_node, value);
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
    
    template <typename... Args>
    void emplace_back(Args&&... args) {
        Node* new_node = node_traits::allocate(allocator_, 1);
        try {
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
    
    void clear() noexcept {
        while (head_) {
            Node* next = head_->next;
            node_traits::destroy(allocator_, head_);
            node_traits::deallocate(allocator_, head_, 1);
            head_ = next;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }
    
    // Вспомогательные методы
    size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
    
    // Итераторы
    iterator begin() noexcept { return iterator(head_); }
    iterator end() noexcept { return iterator(nullptr); }
    const_iterator begin() const noexcept { return const_iterator(head_); }
    const_iterator end() const noexcept { return const_iterator(nullptr); }
    const_iterator cbegin() const noexcept { return const_iterator(head_); }
    const_iterator cend() const noexcept { return const_iterator(nullptr); }
};

#endif