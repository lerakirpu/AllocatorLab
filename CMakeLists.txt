cmake_minimum_required(VERSION 3.15)
project(AllocatorLab VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(STATUS "Компилятор: GCC (MinGW)")
    set(USING_GCC TRUE)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message(STATUS "Компилятор: MSVC")
    set(USING_MSVC TRUE)

    add_compile_options(/W4 /WX- /wd4244 /wd4996) 
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Компилятор: Clang")
    set(USING_CLANG TRUE)
else()
    message(STATUS "Компилятор: ${CMAKE_CXX_COMPILER_ID}")
endif()

if(USING_GCC)
    # Флаги для GCC/MinGW
    add_compile_options(-Wall -Wextra -Wpedantic)
    add_compile_options(-Wno-unused-parameter)
    add_compile_options(-fdiagnostics-color=always)
    
    # Флаги для Debug
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
    
    if(WIN32)
        add_compile_options(-static)
        add_compile_options(-static-libgcc)
        add_compile_options(-static-libstdc++)
    endif()
    
elseif(USING_MSVC)
    add_compile_options(/W4 /WX- /wd4244 /wd4996)  
    
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /D_DEBUG /Zi /MTd")
    
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG /MT")
    
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2 /Zi /DNDEBUG /MT")
endif()

# Основной исполняемый файл
add_executable(allocator_lab
    main.cpp
)

# Подключаем заголовочные файлы
target_include_directories(allocator_lab PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(allocator_lab PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
)

if(WIN32 AND USING_GCC)
    target_link_options(allocator_lab PRIVATE -static)
endif()

install(TARGETS allocator_lab
    RUNTIME DESTINATION bin
)

message(STATUS "Проект: ${PROJECT_NAME}")
message(STATUS "Версия: ${PROJECT_VERSION}")
message(STATUS "Выходной файл: allocator_lab")