cmake_minimum_required(VERSION 3.15)
project(AllocatorLab VERSION 1.0.0)

# Явно указываем C++ стандарт
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Определяем компилятор
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(STATUS "Компилятор: GCC (MinGW)")
    set(USING_GCC TRUE)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message(STATUS "Компилятор: MSVC")
    set(USING_MSVC TRUE)
else()
    message(STATUS "Компилятор: ${CMAKE_CXX_COMPILER_ID}")
endif()

# Разные флаги для разных компиляторов
if(USING_GCC)
    # Флаги для GCC/MinGW
    add_compile_options(-Wall -Wextra -Wpedantic)
    add_compile_options(-Wno-unused-parameter)  # ОТКЛЮЧАЕМ ПРЕДУПРЕЖДЕНИЕ ЗДЕСЬ
    add_compile_options(-fdiagnostics-color=always)
    
    # Флаги для Debug
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEBUG")
    
    # Флаги для Release
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    
    # Флаги для RelWithDebInfo
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
    
    # Для Windows с MinGW - статическая линковка
    if(WIN32)
        add_compile_options(-static)
        add_compile_options(-static-libgcc)
        add_compile_options(-static-libstdc++)
    endif()
    
elseif(USING_MSVC)
    # Флаги для MSVC
    add_compile_options(/W4 /WX)
    
    # Флаги для Debug
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /D_DEBUG /Zi")
    
    # Флаги для Release
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
    
    # Статическая линковка runtime
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

# Основной исполняемый файл
add_executable(allocator_lab
    main.cpp
)

# Установка выходной директории
set_target_properties(allocator_lab PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Для Windows: дополнительные опции
if(WIN32 AND USING_GCC)
    target_link_options(allocator_lab PRIVATE -static)
endif()

# Инсталляция (опционально)
install(TARGETS allocator_lab
    RUNTIME DESTINATION bin
)