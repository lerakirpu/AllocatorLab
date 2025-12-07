#include <iostream>
#include <map>
#include "my_allocator.h"
#include "my_container.h"

// Функция для вычисления факториала - возвращаем int
int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

int main() {
    try {
        // создание экземпляра std::map<int, int>
        std::cout << "# Стандартный map:\n";
        std::map<int, int> map1;
        
        // заполнение 10 элементами
        for (int i = 0; i < 10; ++i) {
            map1[i] = factorial(i);
        }
        
        // вывод стандартного map
        for (const auto& [key, value] : map1) {
            std::cout << key << " " << value << "\n";
        }
        
        // создание std::map с аллокатором
        std::cout << "\nСтандартный map с моим аллокатором:\n";
        using MapAllocator = my_allocator<std::pair<const int, int>, 10>;
        std::map<int, int, std::less<int>, MapAllocator> map2;
        
        // заполнение 10 элементами
        for (int i = 0; i < 10; ++i) {
            map2[i] = factorial(i);
        }
        
        // вывод map с аллокатором
        for (const auto& [key, value] : map2) {
            std::cout << key << " " << value << "\n";
        }
        
        // создание своего контейнера для int
        std::cout << "\nМой контейнер со стандартным аллокатором:\n";
        my_container<int> container1;
        
        // заполнение 10 элементами
        for (int i = 0; i < 10; ++i) {
            container1.push_back(i);
        }
        
        // вывод container1
        bool first = true;
        for (const auto& value : container1) {
            if (!first) std::cout << " ";
            std::cout << value;
            first = false;
        }
        std::cout << "\n";
        
        // создание своего контейнера с аллокатором 
        std::cout << "\n# Мой контейнер с моим аллокатором:\n\n";
        my_container<int, my_allocator<int, 10>> container2;
        
        // заполнение 10 элементами
        for (int i = 0; i < 10; ++i) {
            container2.push_back(i);
        }
        
        first = true;
        for (const auto& value : container2) {
            if (!first) std::cout << " ";
            std::cout << value;
            first = false;
        }
        std::cout << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}